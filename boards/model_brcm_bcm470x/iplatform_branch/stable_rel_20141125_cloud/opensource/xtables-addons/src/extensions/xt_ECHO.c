/*
 *	"ECHO" (RFC 862) target extension for Xtables
 *	Sample module for "Writing your own Netfilter Modules"
 *	Copyright © Jan Engelhardt <jengelh [at] medozas de>, 2008-2011
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License; either
 *	version 2 of the License, or any later version, as published by the
 *	Free Software Foundation.
 */

#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/netfilter/x_tables.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#	include <linux/netfilter_bridge.h>
#endif
#include <net/ip.h>
#include <net/ip6_route.h>
#include <net/route.h>
#include <net/ip6_checksum.h>
#include "compat_xtables.h"

static unsigned int
echo_tg6(struct sk_buff **poldskb, const struct xt_action_param *par)
{
	const struct sk_buff *oldskb = *poldskb;
	const struct udphdr *oldudp;
	const struct ipv6hdr *oldip;
	struct udphdr *newudp, oldudp_buf;
	struct ipv6hdr *newip;
	struct sk_buff *newskb;
	unsigned int data_len;
	void *payload;
	struct flowi6 fl;
	struct dst_entry *dst = NULL;
	struct net *net = dev_net((par->in != NULL) ? par->in : par->out);

	/* This allows us to do the copy operation in fewer lines of code. */
	if (skb_linearize(*poldskb) < 0)
		return NF_DROP;

	oldip  = ipv6_hdr(oldskb);
	oldudp = skb_header_pointer(oldskb, par->thoff,
	         sizeof(*oldudp), &oldudp_buf);
	if (oldudp == NULL)
		return NF_DROP;
	if (ntohs(oldudp->len) <= sizeof(*oldudp))
		return NF_DROP;

	newskb = alloc_skb(LL_MAX_HEADER + sizeof(*newip) +
	         ntohs(oldudp->len), GFP_ATOMIC);
	if (newskb == NULL)
		return NF_DROP;

	skb_reserve(newskb, LL_MAX_HEADER);
	newskb->protocol = oldskb->protocol;

	skb_reset_network_header(newskb);
	newip = (void *)skb_put(newskb, sizeof(*newip));
	newip->version  = oldip->version;
	newip->priority = oldip->priority;
	memcpy(newip->flow_lbl, oldip->flow_lbl, sizeof(newip->flow_lbl));
	newip->nexthdr  = par->target->proto;
	newip->saddr    = oldip->daddr;
	newip->daddr    = oldip->saddr;

	skb_reset_transport_header(newskb);
	newudp = (void *)skb_put(newskb, sizeof(*newudp));
	newudp->source = oldudp->dest;
	newudp->dest   = oldudp->source;
	newudp->len    = oldudp->len;

	data_len = htons(oldudp->len) - sizeof(*oldudp);
	payload  = skb_header_pointer(oldskb, par->thoff +
	           sizeof(*oldudp), data_len, NULL);
	memcpy(skb_put(newskb, data_len), payload, data_len);

#if 0
	/*
	 * Since no fields are modified (we just swapped things around),
	 * this works too in our specific echo case.
	 */
	newudp->check = oldudp->check;
#else
	newudp->check = 0;
	newudp->check = csum_ipv6_magic(&newip->saddr, &newip->daddr,
	                ntohs(newudp->len), IPPROTO_UDP,
	                csum_partial(newudp, ntohs(newudp->len), 0));
#endif

	memset(&fl, 0, sizeof(fl));
	fl.flowi6_proto = newip->nexthdr;
	memcpy(&fl.saddr, &newip->saddr, sizeof(fl.saddr));
	memcpy(&fl.daddr, &newip->daddr, sizeof(fl.daddr));
	fl.fl6_sport = newudp->source;
	fl.fl6_dport = newudp->dest;
	security_skb_classify_flow((struct sk_buff *)oldskb, flowi6_to_flowi(&fl));
	dst = ip6_route_output(net, NULL, &fl);
	if (dst == NULL || dst->error != 0) {
		dst_release(dst);
		goto free_nskb;
	}

	skb_dst_set(newskb, dst);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	newip->hop_limit = ip6_dst_hoplimit(skb_dst(newskb));
#else
	newip->hop_limit = dst_metric(skb_dst(newskb), RTAX_HOPLIMIT);
#endif
	newskb->ip_summed = CHECKSUM_NONE;

	/* "Never happens" (?) */
	if (newskb->len > dst_mtu(skb_dst(newskb)))
		goto free_nskb;

	nf_ct_attach(newskb, *poldskb);
	ip6_local_out(newskb);
	return NF_DROP;

 free_nskb:
	kfree_skb(newskb);
	return NF_DROP;
}

static unsigned int
echo_tg4(struct sk_buff **poldskb, const struct xt_action_param *par)
{
	const struct sk_buff *oldskb = *poldskb;
	const struct udphdr *oldudp;
	const struct iphdr *oldip;
	struct udphdr *newudp, oldudp_buf;
	struct iphdr *newip;
	struct sk_buff *newskb;
	unsigned int data_len;
	void *payload;

	/* This allows us to do the copy operation in fewer lines of code. */
	if (skb_linearize(*poldskb) < 0)
		return NF_DROP;

	oldip  = ip_hdr(oldskb);
	oldudp = skb_header_pointer(oldskb, par->thoff,
	         sizeof(*oldudp), &oldudp_buf);
	if (oldudp == NULL)
		return NF_DROP;
	if (ntohs(oldudp->len) <= sizeof(*oldudp))
		return NF_DROP;

	newskb = alloc_skb(LL_MAX_HEADER + sizeof(*newip) +
	         ntohs(oldudp->len), GFP_ATOMIC);
	if (newskb == NULL)
		return NF_DROP;

	skb_reserve(newskb, LL_MAX_HEADER);
	newskb->protocol = oldskb->protocol;

	skb_reset_network_header(newskb);
	newip = (void *)skb_put(newskb, sizeof(*newip));
	newip->version  = oldip->version;
	newip->ihl      = sizeof(*newip) / 4;
	newip->tos      = oldip->tos;
	newip->id       = 0;
	newip->frag_off = htons(IP_DF);
	newip->protocol = oldip->protocol;
	newip->check    = 0;
	newip->saddr    = oldip->daddr;
	newip->daddr    = oldip->saddr;

	skb_reset_transport_header(newskb);
	newudp = (void *)skb_put(newskb, sizeof(*newudp));
	newudp->source = oldudp->dest;
	newudp->dest   = oldudp->source;
	newudp->len    = oldudp->len;

	data_len = htons(oldudp->len) - sizeof(*oldudp);
	payload  = skb_header_pointer(oldskb, par->thoff +
	           sizeof(*oldudp), data_len, NULL);
	memcpy(skb_put(newskb, data_len), payload, data_len);

#if 0
	/*
	 * Since no fields are modified (we just swapped things around),
	 * this works too in our specific echo case.
	 */
	newudp->check = oldudp->check;
#else
	newudp->check = 0;
	newudp->check = csum_tcpudp_magic(newip->saddr, newip->daddr,
	                ntohs(newudp->len), IPPROTO_UDP,
	                csum_partial(newudp, ntohs(newudp->len), 0));
#endif

	/* ip_route_me_harder expects the skb's dst to be set */
	skb_dst_set(newskb, dst_clone(skb_dst(oldskb)));

	if (ip_route_me_harder(&newskb, RTN_UNSPEC) != 0)
		goto free_nskb;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)
	newip->ttl = ip4_dst_hoplimit(skb_dst(newskb));
#else
	newip->ttl = dst_metric(skb_dst(newskb), RTAX_HOPLIMIT);
#endif
	newskb->ip_summed = CHECKSUM_NONE;

	/* "Never happens" (?) */
	if (newskb->len > dst_mtu(skb_dst(newskb)))
		goto free_nskb;

	nf_ct_attach(newskb, *poldskb);
	ip_local_out(newskb);
	return NF_DROP;

 free_nskb:
	kfree_skb(newskb);
	return NF_DROP;
}

static struct xt_target echo_tg_reg[] __read_mostly = {
	{
		.name       = "ECHO",
		.revision   = 0,
		.family     = NFPROTO_IPV6,
		.proto      = IPPROTO_UDP,
		.table      = "filter",
		.target     = echo_tg6,
		.me         = THIS_MODULE,
	},
	{
		.name       = "ECHO",
		.revision   = 0,
		.family     = NFPROTO_IPV4,
		.proto      = IPPROTO_UDP,
		.table      = "filter",
		.target     = echo_tg4,
		.me         = THIS_MODULE,
	},
};

static int __init echo_tg_init(void)
{
	return xt_register_targets(echo_tg_reg, ARRAY_SIZE(echo_tg_reg));
}

static void __exit echo_tg_exit(void)
{
	return xt_unregister_targets(echo_tg_reg, ARRAY_SIZE(echo_tg_reg));
}

module_init(echo_tg_init);
module_exit(echo_tg_exit);
MODULE_AUTHOR("Jan Engelhardt <jengelh@medozas.de>");
MODULE_DESCRIPTION("Xtables: ECHO diagnosis target");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ip6t_ECHO");
MODULE_ALIAS("ipt_ECHO");
