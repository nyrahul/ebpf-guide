/*
 * Do not allow TCP port 8080 to connect by dropping corr pkts.
 *
 * Check: block-tcp-8080.rst for details
 */

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

// Sizeof all headers till TCP
#define TOTSZ (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr))

#define htons(X)	((((uint16_t)X)&0xff00>>16)|(((uint16_t)X)&0x00ff<<16))

int xdp_drop(struct xdp_md *ctx)
{
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    struct iphdr *ip = data + sizeof(struct ethhdr);
    struct tcphdr *tcph = data + sizeof(struct ethhdr) + sizeof(struct iphdr);

    if (data + TOTSZ > data_end) {
        return XDP_PASS;
    }

    if (ip->protocol == IPPROTO_TCP && tcph->source == htons(8080)) {
        return XDP_DROP;
    }

    return XDP_PASS;
}
