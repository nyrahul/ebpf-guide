/*
 * Do not allow a container to generate spoofed packet
 */

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

#include "drop-spoofs.h"

#ifndef lock_xadd
#define lock_xadd(ptr, val) ((void)__sync_fetch_and_add(ptr, val))
#endif

#define TC_ACT_OK 				0
#define TC_ACT_STOLEN 			4

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, MAX_IFACES);
	__type(key, uint32_t);
	__type(value, uint32_t);
} iface_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 2);
	__type(key, uint32_t);
	__type(value, uint32_t);
} stat_map SEC(".maps");

static __inline void update_stat(uint32_t act)
{
    uint32_t *val;

    val = bpf_map_lookup_elem(&stat_map, &act);
    if (val) lock_xadd(val, 1);
}

SEC("egress")
int tc_egress(struct __sk_buff *skb)
{
    uint32_t act = ACT_ALLOW;
    uint32_t idx;
    uint32_t *val;
    void *data = (void *)(long)skb->data;
    void *data_end = (void *)(long)skb->data_end;
    struct iphdr *ip = data + sizeof(struct ethhdr);

    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end) {
        return TC_ACT_OK;
    }

    idx = skb->ifindex;
    val = bpf_map_lookup_elem(&iface_map, &idx);
    if (val && *val && (ip->saddr & 0x00ffffff) != (*val & 0x00ffffff)) {
        act = ACT_DROP;
    }

    update_stat(act);
    return act == ACT_ALLOW?TC_ACT_OK:TC_ACT_STOLEN;
}

char __license[] SEC("license") = "GPL";
