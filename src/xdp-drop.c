#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

#define htons(x) ((__be16)___constant_swab16((x)))

// Sizeof all headers till UDP
#define TOTSZ (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr))

int xdp_drop(struct xdp_md *ctx)
{
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    struct iphdr *ip = data + sizeof(struct ethhdr);
    struct udphdr *udph = data + sizeof(struct ethhdr) + sizeof(struct iphdr);

    if (data + TOTSZ > data_end) {
        return XDP_PASS;
    }

    if (ip->protocol == IPPROTO_UDP && udph->dest == htons(1234)) {
        return XDP_DROP;
    }

    return XDP_PASS;
}
