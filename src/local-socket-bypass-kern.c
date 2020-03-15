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

struct bpf_map_def SEC("maps") sock_ops = {
    .type        = BPF_MAP_TYPE_SOCKMAP,
    .key_size    = sizeof(int),
    .value_size  = sizeof(unsigned int),
    .max_entries = 2,
};

SEC("sockops")
int sock_map_update(struct bpf_sock_ops *ops)
{
    int op;
    op = (int) ops->op;

    bpf_printk("XYZ BPF command: %d\n", op);
    if (op == BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB || op == BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB) {
        uint32_t idx = 0;
        bpf_printk("Calling UPDATE: %d %d\n", op, BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB);
        bpf_sock_map_update(ops, &sock_ops, &idx, BPF_ANY);
    }

    return 0;
}

char _license[] SEC("license") = "GPL";
int  _version   SEC("version") = LINUX_VERSION_CODE;
