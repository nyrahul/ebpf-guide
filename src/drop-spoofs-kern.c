/*
 * Do not allow a container to generate spoofed packet
 */

#include <stdint.h>
#include <linux/bpf.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/pkt_cls.h>
#include <linux/if_ether.h>
#include "drop-spoofs.h"

#ifndef SEC
#define SEC(NAME) __attribute__((section(NAME), used))
#endif

#ifndef lock_xadd
#define lock_xadd(ptr, val) ((void)__sync_fetch_and_add(ptr, val))
#endif

#ifndef BPF_FUNC
#define BPF_FUNC(NAME, ...) (*NAME)(__VA_ARGS__) = (void *)BPF_FUNC_##NAME
#endif

static void *BPF_FUNC(map_lookup_elem, void *map, const void *key);

#define PIN_GLOBAL_NS           2

struct bpf_elf_map {
    __u32 type;
    __u32 size_key;
    __u32 size_value;
    __u32 max_elem;
    __u32 flags;
    __u32 id;
    __u32 pinning;
};

struct bpf_elf_map iface_map SEC("maps") = {
    .type           = BPF_MAP_TYPE_ARRAY,
    .size_key       = sizeof(uint32_t),
    .size_value     = sizeof(uint32_t),
    .max_elem       = MAX_IFACES,
    .pinning        = PIN_GLOBAL_NS,
};

struct bpf_elf_map stat_map SEC("maps") = {
    .type           = BPF_MAP_TYPE_ARRAY,
    .size_key       = sizeof(uint32_t),
    .size_value     = sizeof(uint32_t),
    .max_elem       = 2,
    .pinning        = PIN_GLOBAL_NS,
};

static __inline void update_stat(uint32_t act)
{
    uint32_t *val;

    val = map_lookup_elem(&stat_map, &act);
    if (val) lock_xadd(val, 1);
}

SEC("egress")
int tc_egress(struct __sk_buff *skb)
{
    uint32_t act = ACT_DROP;
    uint32_t idx;
    uint32_t *val;
    void *data = (void *)(long)skb->data;
    void *data_end = (void *)(long)skb->data_end;
    struct iphdr *ip = data + sizeof(struct ethhdr);

    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end) {
        return TC_ACT_OK;
    }

    idx = skb->ifindex;
    val = map_lookup_elem(&iface_map, &idx);
    if (val && ip->saddr == *val) {
        act = ACT_ALLOW;
    }

    update_stat(act);
    return act == ACT_ALLOW?TC_ACT_OK:TC_ACT_STOLEN;
}

char __license[] SEC("license") = "GPL";
