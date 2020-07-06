/*
 *	"condition" match extension for Xtables
 *
 *	Description: This module allows firewall rules to match using
 *	condition variables available through procfs.
 *
 *	Authors:
 *	Stephane Ouellette <ouellettes [at] videotron ca>, 2002-10-22
 *	Massimiliano Hofer <max [at] nucleus it>, 2006-05-15
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License; either version 2
 *	or 3 of the License, as published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/netfilter/x_tables.h>
#include <asm/uaccess.h>
#include "xt_condition.h"
#include "compat_xtables.h"

#ifndef CONFIG_PROC_FS
#	error "proc file system support is required for this module"
#endif

/* Defaults, these can be overridden on the module command-line. */
static unsigned int condition_list_perms = S_IRUGO | S_IWUSR;
static unsigned int condition_uid_perms = 0;
static unsigned int condition_gid_perms = 0;

MODULE_AUTHOR("Stephane Ouellette <ouellettes@videotron.ca>");
MODULE_AUTHOR("Massimiliano Hofer <max@nucleus.it>");
MODULE_AUTHOR("Jan Engelhardt <jengelh@medozas.de>");
MODULE_DESCRIPTION("Allows rules to match against condition variables");
MODULE_LICENSE("GPL");
module_param(condition_list_perms, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(condition_list_perms, "permissions on /proc/net/nf_condition/* files");
module_param(condition_uid_perms, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(condition_uid_perms, "user owner of /proc/net/nf_condition/* files");
module_param(condition_gid_perms, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(condition_gid_perms, "group owner of /proc/net/nf_condition/* files");
MODULE_ALIAS("ipt_condition");
MODULE_ALIAS("ip6t_condition");

struct condition_variable {
	struct list_head list;
	struct proc_dir_entry *status_proc;
	unsigned int refcount;
	bool enabled;
};

/* proc_lock is a user context only semaphore used for write access */
/*           to the conditions' list.                               */
static DEFINE_MUTEX(proc_lock);

static LIST_HEAD(conditions_list);
static struct proc_dir_entry *proc_net_condition;

static int condition_proc_read(char __user *buffer, char **start, off_t offset,
                               int length, int *eof, void *data)
{
	const struct condition_variable *var = data;

	buffer[0] = var->enabled ? '1' : '0';
	buffer[1] = '\n';
	if (length >= 2)
		*eof = true;
	return 2;
}

static int condition_proc_write(struct file *file, const char __user *buffer,
                                unsigned long length, void *data)
{
	struct condition_variable *var = data;
	char newval;

	if (length > 0) {
		if (get_user(newval, buffer) != 0)
			return -EFAULT;
		/* Match only on the first character */
		switch (newval) {
		case '0':
			var->enabled = false;
			break;
		case '1':
			var->enabled = true;
			break;
		}
	}
	return length;
}

static bool
condition_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_condition_mtinfo *info = par->matchinfo;
	const struct condition_variable *var   = info->condvar;

	return var->enabled ^ info->invert;
}

static int condition_mt_check(const struct xt_mtchk_param *par)
{
	struct xt_condition_mtinfo *info = par->matchinfo;
	struct condition_variable *var;

	/* Forbid certain names */
	if (*info->name == '\0' || *info->name == '.' ||
	    info->name[sizeof(info->name)-1] != '\0' ||
	    memchr(info->name, '/', sizeof(info->name)) != NULL) {
		printk(KERN_INFO KBUILD_MODNAME ": name not allowed or too "
		       "long: \"%.*s\"\n", (unsigned int)sizeof(info->name),
		       info->name);
		return -EINVAL;
	}
	/*
	 * Let's acquire the lock, check for the condition and add it
	 * or increase the reference counter.
	 */
	mutex_lock(&proc_lock);
	list_for_each_entry(var, &conditions_list, list) {
		if (strcmp(info->name, var->status_proc->name) == 0) {
			var->refcount++;
			mutex_unlock(&proc_lock);
			info->condvar = var;
			return 0;
		}
	}

	/* At this point, we need to allocate a new condition variable. */
	var = kmalloc(sizeof(struct condition_variable), GFP_KERNEL);
	if (var == NULL) {
		mutex_unlock(&proc_lock);
		return -ENOMEM;
	}

	/* Create the condition variable's proc file entry. */
	var->status_proc = create_proc_entry(info->name, condition_list_perms,
	                   proc_net_condition);
	if (var->status_proc == NULL) {
		kfree(var);
		mutex_unlock(&proc_lock);
		return -ENOMEM;
	}

	var->refcount = 1;
	var->enabled  = false;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 29)
	var->status_proc->owner = THIS_MODULE;
#endif
	var->status_proc->data  = var;
	wmb();
	var->status_proc->read_proc  = condition_proc_read;
	var->status_proc->write_proc = condition_proc_write;
	list_add(&var->list, &conditions_list);
	var->status_proc->uid = condition_uid_perms;
	var->status_proc->gid = condition_gid_perms;
	mutex_unlock(&proc_lock);
	info->condvar = var;
	return 0;
}

static void condition_mt_destroy(const struct xt_mtdtor_param *par)
{
	const struct xt_condition_mtinfo *info = par->matchinfo;
	struct condition_variable *var = info->condvar;

	mutex_lock(&proc_lock);
	if (--var->refcount == 0) {
		list_del(&var->list);
		remove_proc_entry(var->status_proc->name, proc_net_condition);
		mutex_unlock(&proc_lock);
		kfree(var);
		return;
	}
	mutex_unlock(&proc_lock);
}

static struct xt_match condition_mt_reg[] __read_mostly = {
	{
		.name       = "condition",
		.revision   = 1,
		.family     = NFPROTO_IPV4,
		.matchsize  = sizeof(struct xt_condition_mtinfo),
		.match      = condition_mt,
		.checkentry = condition_mt_check,
		.destroy    = condition_mt_destroy,
		.me         = THIS_MODULE,
	},
	{
		.name       = "condition",
		.revision   = 1,
		.family     = NFPROTO_IPV6,
		.matchsize  = sizeof(struct xt_condition_mtinfo),
		.match      = condition_mt,
		.checkentry = condition_mt_check,
		.destroy    = condition_mt_destroy,
		.me         = THIS_MODULE,
	},
};

static const char *const dir_name = "nf_condition";

static int __init condition_mt_init(void)
{
	int ret;

	mutex_init(&proc_lock);
	proc_net_condition = proc_mkdir(dir_name, init_net__proc_net);
	if (proc_net_condition == NULL)
		return -EACCES;

	ret = xt_register_matches(condition_mt_reg, ARRAY_SIZE(condition_mt_reg));
	if (ret < 0) {
		remove_proc_entry(dir_name, init_net__proc_net);
		return ret;
	}

	return 0;
}

static void __exit condition_mt_exit(void)
{
	xt_unregister_matches(condition_mt_reg, ARRAY_SIZE(condition_mt_reg));
	remove_proc_entry(dir_name, init_net__proc_net);
}

module_init(condition_mt_init);
module_exit(condition_mt_exit);
