/*
 * Do not allow TCP port 8080 to connect by dropping corr pkts.
 *
 * Check: block-tcp-8080.rst for details
 */

#include <uapi/linux/bpf.h>
#include <uapi/linux/in.h>
#include <uapi/linux/if_ether.h>
#include <uapi/linux/ip.h>
#include <uapi/linux/tcp.h>

// Sizeof all headers till TCP
#define TOTSZ (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr))

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
