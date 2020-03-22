/*
 * Completely bypass kernel TCP/IP stack for local socket.
 * Note:
 * 1. sockmap can be used only for TCP sockets
 * 2. since tcp/ip stack is bypassed, underneath tooling such as tcpdump and
 *    other OAM/stats collection will not work for these bypassed sockets.
 * 3. 
 */

#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/version.h>

#include "bpf_helpers.h"

#define SRV_IDX  0
#define CLI_IDX  1

struct bpf_map_def SEC("maps") skmap = {
    .type        = BPF_MAP_TYPE_SOCKMAP,
    .key_size    = sizeof(int),
    .value_size  = sizeof(unsigned int),
    .max_entries = 2,
};

SEC("sk_skb1")
int prog1(struct __sk_buff *skb)
{
    bpf_printk("lport=%d parser called %d\n", skb->local_port, skb->len);
	return skb->len;
}

SEC("sk_skb2")
int prog2(struct __sk_buff *skb)
{
    int verdict;
    __u32 lport = skb->local_port;
	uint32_t idx = CLI_IDX;

    if (lport == 12345) {
        idx = SRV_IDX;
    }
	verdict = bpf_sk_redirect_map(skb, &skmap, idx, BPF_F_INGRESS);
    bpf_printk("lport=%d verdict %d\n", lport, skb->len);
    return verdict;
}

SEC("sockops")
int sock_map_update(struct bpf_sock_ops *ops)
{
    __u32 lport = ops->local_port;
    __u32 rport = ops->remote_port;
    __u32 lip = ops->local_ip4;
    int op = (int) ops->op;

    if (lip != 0x100007f) {
        return 0;
    }

    if (op == BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB || op == BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB) {
        int idx = CLI_IDX, ret;
        if (lport == 12345) {
            idx = SRV_IDX;
        }
        bpf_printk("%d UPDATE: lport=%d\n", idx, lport);
        ret = bpf_sock_map_update(ops, &skmap, &idx, BPF_NOEXIST);
        if (ret) {
            bpf_printk("FAILED bpf_sock_map_update ret=%d\n", ret);
        }
    }

    return 0;
}

char _license[] SEC("license") = "GPL";
int  _version   SEC("version") = LINUX_VERSION_CODE;
