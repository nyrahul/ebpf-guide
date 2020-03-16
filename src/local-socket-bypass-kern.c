/*
 * Completely bypass kernel TCP/IP stack for local socket.
 * Note:
 * 1. sockmap can be used only for TCP sockets
 * 2. since tcp/ip stack is bypassed, underneath tooling such as tcpdump and
 *    other OAM/stats collection will not work for these bypassed sockets.
 * 3. 
 */

#include <uapi/linux/bpf.h>
#include <linux/version.h>

#include "bpf_helpers.h"

struct bpf_map_def SEC("maps") skmap = {
    .type        = BPF_MAP_TYPE_SOCKMAP,
    .key_size    = sizeof(int),
    .value_size  = sizeof(unsigned int),
    .max_entries = 2,
};

#if 1
SEC("sk_skb1")
int prog1(struct __sk_buff *skb)
{
    bpf_printk("parser called\n");
	return skb->len;
}

SEC("sk_skb2")
int prog2(struct __sk_buff *skb)
{
	uint32_t idx = 0;
    bpf_printk("verdict called\n");
	return bpf_sk_redirect_map(skb, &skmap, idx, 0);
}
#endif

SEC("sockops")
int sock_map_update(struct bpf_sock_ops *ops)
{
    int op;
    op = (int) ops->op;

    if (op == BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB || op == BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB) {
        uint32_t idx = 0;
        int ret;
        bpf_printk("Calling UPDATE: loc=0x%x:%d rem=0x%x\n",
                ops->local_ip4, ops->local_port, ops->remote_ip4);
        ret = bpf_sock_map_update(ops, &skmap, &idx, BPF_ANY);
        if (ret) {
            bpf_printk("FAILED bpf_sock_map_update ret=%d\n", ret);
        }
    }

    return 0;
}

char _license[] SEC("license") = "GPL";
int  _version   SEC("version") = LINUX_VERSION_CODE;
