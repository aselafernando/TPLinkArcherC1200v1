/* Kernel module to match the port-ranges, trigger related port-ranges,
 * and alters the destination to a local IP address.
 *
 * Copyright (C) 2003, CyberTAN Corporation
 * All Rights Reserved.
 *
 * Description:
 *   This is kernel module for port-triggering.
 *
 *   The module follows the Netfilter framework, called extended packet
 *   matching modules.
 */

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>
#include <linux/version.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/x_tables.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_nat_rule.h>

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))
#include "compat_xtnu.h"
#endif

#include "ipt_TRIGGER.h"

#include <linux/spinlock.h>

#define MUST_BE_READ_LOCKED(l)
#define MUST_BE_WRITE_LOCKED(l)

#define READ_LOCK(l) read_lock_bh(l)
#define WRITE_LOCK(l) spin_lock_bh(l)/*write_lock_bh(l)*/
#define READ_UNLOCK(l)  read_unlock_bh(l)
#define WRITE_UNLOCK(l) spin_unlock_bh(l)/*write_unlock_bh(l)*/


/* This rwlock protects the main hash table, protocol/helper/expected
 *    registrations, conntrack timers*/
#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&nf_conntrack_lock)
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&nf_conntrack_lock)

/* Return pointer to first true entry, if any, or NULL.  A macro
   required to allow inlining of cmpfn. */
#define LIST_FIND(head, cmpfn, type, args...)		\
({							\
	const struct list_head *__i, *__j = NULL;	\
							\
	ASSERT_READ_LOCK(head);				\
	list_for_each(__i, (head))			\
		if (cmpfn((const type)__i , ## args)) {	\
			__j = __i;			\
			break;				\
		}					\
	(type)__j;					\
})


MODULE_AUTHOR("LSZ <www.tplink.com.cn>");
MODULE_DESCRIPTION("Target for port trigger");
MODULE_LICENSE("GPL");


#ifndef NF_IP_PRE_ROUTING
#define NF_IP_PRE_ROUTING 0
#define NF_IP_FORWARD 2
#endif


#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

struct ipt_trigger {
	struct list_head list;		/* Trigger list */
	struct timer_list timeout;	/* Timer for list destroying */
	u_int32_t srcip;		/* Outgoing source address */
	u_int32_t dstip;		/* Outgoing destination address */
	u_int16_t mproto;		/* Trigger protocol */
	u_int16_t rproto;		/* Related protocol */
	struct ipt_trigger_ports ports;	/* Trigger and related ports */
	u_int8_t reply;			/* Confirm a reply connection */
};

LIST_HEAD(trigger_list);
//DECLARE_LOCK(ip_trigger_lock);

static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    WRITE_LOCK(&nf_conntrack_lock);

    /* Need del_timer for race avoidance (may already be dying). */
    if (del_timer(&trig->timeout)) {
	trig->timeout.expires = jiffies + extra_jiffies;
	add_timer(&trig->timeout);
    }

    WRITE_UNLOCK(&nf_conntrack_lock);
}

static void __del_trigger(struct ipt_trigger *trig)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    MUST_BE_WRITE_LOCKED(&nf_conntrack_lock);

     /* delete from 'trigger_list' */
    list_del(&trig->list);
    kfree(trig);
}

static void trigger_timeout(unsigned long ul_trig)
{
    struct ipt_trigger *trig= (void *) ul_trig;

    DEBUGP("trigger list %p timed out\n", trig);
    /*WRITE_LOCK(&nf_conntrack_lock);*/
    __del_trigger(trig);
    /*WRITE_UNLOCK(&nf_conntrack_lock);*/
}

static unsigned int
add_new_trigger(struct ipt_trigger *trig)
{
    struct ipt_trigger *new;

    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__);
    WRITE_LOCK(&nf_conntrack_lock);
    new = (struct ipt_trigger *)
	kmalloc(sizeof(struct ipt_trigger), GFP_ATOMIC);

    if (!new) {
	WRITE_UNLOCK(&nf_conntrack_lock);
	DEBUGP("%s: OOM allocating trigger list\n", __FUNCTION__);
	return -ENOMEM;
    }

    memset(new, 0, sizeof(*trig));
    INIT_LIST_HEAD(&new->list);
    memcpy(new, trig, sizeof(*trig));

    /* add to global table of trigger */
#if 0
    list_prepend(&trigger_list, &new->list);
#else
	list_add(&new->list, &trigger_list);
#endif
    /* add and start timer if required */
    init_timer(&new->timeout);
    new->timeout.data = (unsigned long)new;
    new->timeout.function = trigger_timeout;
    new->timeout.expires = jiffies + (TRIGGER_TIMEOUT * HZ);
    add_timer(&new->timeout);

    WRITE_UNLOCK(&nf_conntrack_lock);

    return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i,
	const u_int16_t proto, const u_int16_t dport, const u_int16_t *rport)
{
    DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
    DEBUGP("%s: Got one, mproto= %d, mport[0..1]=%d, %d.\n", __FUNCTION__,
	    i->mproto, i->ports.mport[0], i->ports.mport[1]);

    return ((i->mproto == proto) 			/* proto is rignt*/
    	&& (i->ports.mport[0] <= dport) 	/* dst port is between the match ports */
	    && (i->ports.mport[1] >= dport)
	    && (i->ports.rport[0] <= rport[0])	/* related ports is contained by old ones */
	    && (i->ports.rport[1] >= rport[1]));
}

static unsigned int
trigger_out(struct sk_buff *skb,
		const struct xt_action_param *param)
{
    const struct ipt_trigger_info *info = param->targinfo;
    struct ipt_trigger trig, *found;
	const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

	DEBUGP("%s: \n", __FUNCTION__);


	/* if the pkt's dst port is not in the range of match ports
	 * nothing happened.
	 */
	if (info->ports.mport[0] > ntohs(tcph->dest) ||
		info->ports.mport[1] < ntohs(tcph->dest))
		return XT_CONTINUE/*IPT_CONTINUE*/;


    DEBUGP("############# %s ############\n", __FUNCTION__);
    /* Check if the trigger range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_out_matched,
	    struct ipt_trigger *, iph->protocol, ntohs(tcph->dest), info->ports.rport);

    if (found) {
		/* Yeah, it exists. We need to update(delay) the destroying timer. */
		trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
		/* In order to allow multiple hosts use the same port range, we update
		   the 'saddr' after previous trigger has a reply connection. */
		if (found->reply)
		    found->srcip = iph->saddr;
    }
    else {
		/* Create new trigger */
		memset(&trig, 0, sizeof(trig));
		trig.srcip = iph->saddr;
		trig.mproto = iph->protocol;
		trig.rproto = info->proto;

		memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));

		// added by lishaozhang
		// add new trig only when the packet's dst port is in the range of mports
		if (info->ports.mport[0] <= ntohs(tcph->dest) &&
			info->ports.mport[1] >= ntohs(tcph->dest))
		// end of added code
		add_new_trigger(&trig);	/* Add the new 'trig' to list 'trigger_list'. */
    }

    return XT_CONTINUE;	/* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i,
	const u_int16_t proto, const u_int16_t dport)
{
    u_int16_t rproto = i->rproto;

	DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
    DEBUGP("%s: Got one, rproto= %d, rport[0..1]=%d, %d.\n", __FUNCTION__,
	    i->rproto, i->ports.rport[0], i->ports.rport[1]);

    if (!rproto)// relate proto is all(0)
		rproto = proto;

    return ((rproto == proto) && (i->ports.rport[0] <= dport)
	    && (i->ports.rport[1] >= dport));
}

static unsigned int
trigger_in(struct sk_buff *skb,
		const struct xt_action_param *param)
{
    struct ipt_trigger *found;
 	const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

	DEBUGP("%s: \n", __FUNCTION__);

//	DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
//   DEBUGP("%s: Got one, rproto= %d, rport[0..1]=%d, %d.\n", __FUNCTION__,
//	    i->rproto, i->ports.rport[0], i->ports.rport[1]);

    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched,
	    struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));
    if (found)
	{
		DEBUGP("############# %s ############\n", __FUNCTION__);
		/* Yeah, it exists. We need to update(delay) the destroying timer. */
		trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
		return NF_ACCEPT;	/* Accept it, or the imcoming packet could be
				   dropped in the FORWARD chain */
    }

    return XT_CONTINUE;	/* Our job is the interception. */
}

static unsigned int
trigger_dnat(struct sk_buff *skb, const struct xt_action_param *param)
{
	int ret;
	struct ipt_trigger *found;
	const struct iphdr *iph = ip_hdr(skb);

	struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	enum nf_nat_manip_type maniptypex = HOOK2MANIP(param->hooknum);

	#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0) || LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
	struct nf_nat_multi_range_compat newrange;
	#else
	struct nf_nat_ipv4_multi_range_compat newrange;
	#endif

	DEBUGP("%s: \n", __FUNCTION__);
	DEBUGP("maniptypex : %d\n\n", maniptypex);

	NF_CT_ASSERT(param->hooknum == NF_IP_PRE_ROUTING);
	/* Check if the trigger-ed range has already existed in 'trigger_list'. */
	found = LIST_FIND(&trigger_list, trigger_in_matched,
			struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

	if (!found || !found->srcip)
		return XT_CONTINUE/*IPT_CONTINUE*/;	/* We don't block any packet. */

	DEBUGP("############# %s ############\n", __FUNCTION__);
	found->reply = 1;	/* Confirm there has been a reply connection. */
	ct = nf_ct_get(skb, &ctinfo);
	NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW));

	DEBUGP("%s: got\n ", __FUNCTION__);
	nf_ct_dump_tuple(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

	/* Alter the destination of imcoming packet. */

	#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0) || LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
	newrange = ((struct nf_nat_multi_range_compat)
    			{ 1, 	
    			   { 
    			   	{ IP_NAT_RANGE_MAP_IPS,
	             		   found->srcip,
	             		   found->srcip,
	             		   { 0 }, 
	             		   { 0 }
				 }
			    } 
			 });
	#else
	newrange = ((struct nf_nat_ipv4_multi_range_compat)
    			{ 1,
    			   {
    			   	{ NF_NAT_RANGE_MAP_IPS,
	             		   found->srcip,
	             		   found->srcip,
	             		   { 0 },
	             		   { 0 }
				 }
			    }
			 });

	#endif
	
	
	DEBUGP("%s:  found->srcip  %x;  found->dstip  %x;param->hooknum %d; \n",
		__FUNCTION__, found->srcip, found->dstip, param->hooknum);

	/* Hand modified range to generic setup. */
	//ret = nf_nat_setup_info(ct, newrange.range, IP_NAT_MANIP_DST);

	ret = nf_nat_setup_info(ct, newrange.range, maniptypex);

	//DEBUGP("%s: ret %d\n ", __FUNCTION__, ret);
	return ret;
}

static unsigned int
trigger_target(struct sk_buff *skb,
		const struct xt_action_param *param)
{
    const struct ipt_trigger_info *info = param->targinfo;
	const struct iphdr *iph = ip_hdr(skb);


    DEBUGP("%s: type = %s\n", __FUNCTION__,
	    (info->type == IPT_TRIGGER_DNAT) ? "dnat" :
	    (info->type == IPT_TRIGGER_IN) ? "in" : "out");

    /* The Port-trigger only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
	return XT_CONTINUE;

    if (info->type == IPT_TRIGGER_OUT)
		return trigger_out(skb, param);
    else if (info->type == IPT_TRIGGER_IN)
		return trigger_in(skb, param);
    else if (info->type == IPT_TRIGGER_DNAT)
    	return trigger_dnat(skb, param);

    return XT_CONTINUE;
}


static int
trigger_check(const struct xt_tgchk_param *param)
{
	const struct ipt_trigger_info *info = param->targinfo;
	struct list_head *cur_item, *tmp_item;

	DEBUGP("%s: \n", __FUNCTION__);

	if ((strcmp(param->table, "mangle") == 0)) {
		DEBUGP("trigger_check: bad table `%s'.\n", param->table);
		return -EINVAL;
	}

	if ( param->target->targetsize != XT_ALIGN(sizeof(*info))) {
		DEBUGP("trigger_check: size %u.\n", param->target->targetsize);
		return -EINVAL;
	}

	if (param->hook_mask & ~((1 << NF_IP_PRE_ROUTING) | (1 << NF_IP_FORWARD))) {
		DEBUGP("trigger_check: bad hooks %x.\n", param->hook_mask);
		return -EINVAL;
	}
	if (nf_ct_l3proto_try_module_get(param->target->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%d\n", param->target->family);
		return -EINVAL;
	}

	if (info->type == IPT_TRIGGER_OUT) {
	    if (!info->ports.mport[0] || !info->ports.rport[0]) {
		DEBUGP("trigger_check: Try 'iptbles -j TRIGGER -h' for help.\n");
		return -EINVAL;
	    }
	}

	/* Empty the 'trigger_list' */
	list_for_each_safe(cur_item, tmp_item, &trigger_list) {
	    struct ipt_trigger *trig = (void *)cur_item;

	    DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
	    del_timer(&trig->timeout);
	    __del_trigger(trig);
	}

	DEBUGP("%s: return 0\n", __FUNCTION__);

	return 0;
}

static struct xt_target ipt_trigger_target = {
	.name		= "TRIGGER",
	.family		= NFPROTO_IPV4,
	.target		= trigger_target,
	.targetsize	= sizeof(struct ipt_trigger_info),
	.checkentry	= trigger_check,
    .me			= THIS_MODULE,
};

static int __init init(void)
{
	int ret;
	DEBUGP(" lqt === 7777\n");
	ret = xt_register_target(&ipt_trigger_target);
	DEBUGP(" init ret:%d\n", ret);

	return ret;
}

static void __exit fini(void)
{
	xt_unregister_target(&ipt_trigger_target);
}

module_init(init);
module_exit(fini);
