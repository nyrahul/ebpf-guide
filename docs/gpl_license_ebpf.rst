eBPF and GPL licensing
======================

eBPF kernel code is tightly coupled with the regular linux kernel code i.e., it
operates with the same priviledges and address space as the kernel. This means
eBPF kernel code needs to adhere to the licensing restrictions of the kernel
components whichever are applicable. For ebpf these kernel components are
helper_ functions.

eBPF helper_ functions allows one to invoke kernel functions. These helper
functions can either be GPLed or not and they have to explicitly define
their license. For e.g.,::
    static const struct bpf_func_proto bpf_probe_read_proto = {
        .func       = bpf_probe_read,
        .gpl_only   = true,
        .ret_type   = RET_INTEGER,
        .arg1_type  = ARG_PTR_TO_UNINIT_MEM,
        .arg2_type  = ARG_CONST_SIZE_OR_ZERO,
        .arg3_type  = ARG_ANYTHING,
    };

Notice the ``gpl_only`` flag which is set to true in the above helper function,
which means that any eBPF code using ``bpf_probe_read`` needs to be declared as
GPL too using:
.. code:: c
    char __license[] __attribute__((section("license"), used)) = "GPL";

What happens if you try to use GPL-only helper in a eBPF not declared as "GPL"?
-------------------------------------------------------------------------------

In-kernel verifier will spit out the bytecode with an error, for e.g.,::
    Verifier analysis:
    0: (bf) r6 = r1
    1: (61) r8 = *(u32 *)(r6 +80)
    2: (61) r7 = *(u32 *)(r6 +76)
    3: (b7) r1 = 686200
    4: (63) *(u32 *)(r10 -12) = r1
    5: (bf) r1 = r10
    6: (07) r1 += -12
    7: (b7) r2 = 4
    8: (85) call bpf_trace_printk#6
    cannot call GPL-restricted function from non-GPL compatible program
    processed 9 insns (limit 1000000) max_states_per_insn 0 total_states 0 peak_states 0 mark_read 0

In this case I used ``bpf_trace_printk`` without declaring the "GPL" license
for ebpf kernel code.

What about the user-space code which links with ebpf kernel code using maps?
----------------------------------------------------------------------------
This is tricky! GPL license says that if the userspace app is tightly coupled
with the kernel space app using for e.g, shared memory or highly dependent APIs
then the userspace app must be made available as GPL code. However, the meaning
of "tight coupling" can be left open to interpretation even though the GPL
license definition tries to clarify the commonly used scenarios.

In general, any pair of code modules of a project are always linked directly or
indirectly through some communication primitives. It could be sockets, RPCs,
pipes, or shared memory. It could be directly or not directly linked for e.g.
using shared library during link time or dynanmically linked using libdl
(dlsym/dlopen) or statically linked.

Most of the ebpf user-space code I have seen is not licensed as GPL (for e.g.,
from Cilium_ which is licensed as Apache 2.0).

List of GPLed vs non-GPLed helpers
----------------------------------
Assuming Kernel v5.3.0

GPLed::

    bpf_override_return
    bpf_probe_read
    bpf_probe_write_user
    bpf_trace_printk
    bpf_perf_event_read
    bpf_perf_event_read_value
    bpf_perf_event_output
    bpf_get_current_task
    bpf_probe_read_str
    bpf_perf_event_output_tp
    bpf_get_stackid_tp
    bpf_get_stack_tp
    bpf_perf_prog_read_value
    bpf_perf_event_output_raw_tp
    bpf_get_stackid_raw_tp
    bpf_get_stack_raw_tp
    bpf_rc_repeat
    bpf_rc_keydown
    bpf_rc_pointer_rel
    bpf_skb_event_output
    bpf_xdp_event_output
    bpf_sockopt_event_output
    bpf_xdp_fib_lookup
    bpf_skb_fib_lookup
    bpf_tcp_check_syncookie

Non-GPLed::
    bpf_current_task_under_cgroup
    bpf_send_signal
    bpf_sysctl_get_name
    bpf_sysctl_get_current_value
    bpf_sysctl_get_new_value
    bpf_sysctl_set_new_value
    bpf_get_raw_cpu_id
    bpf_skb_store_bytes
    bpf_skb_load_bytes
    bpf_flow_dissector_load_bytes
    bpf_skb_load_bytes_relative
    bpf_skb_pull_data
    bpf_sk_fullsock
    sk_skb_pull_data
    bpf_l3_csum_replace
    bpf_l4_csum_replace
    bpf_csum_diff
    bpf_csum_update
    bpf_clone_redirect
    bpf_redirect
    bpf_msg_apply_bytes
    bpf_msg_cork_bytes
    bpf_msg_pull_data
    bpf_msg_push_data
    bpf_msg_pop_data
    bpf_get_cgroup_classid
    bpf_get_route_realm
    bpf_get_hash_recalc
    bpf_set_hash_invalid
    bpf_set_hash
    bpf_skb_vlan_push
    bpf_skb_vlan_pop
    bpf_skb_change_proto
    bpf_skb_change_type
    bpf_skb_adjust_room
    bpf_skb_change_tail
    sk_skb_change_tail
    bpf_skb_change_head
    sk_skb_change_head
    bpf_xdp_adjust_head
    bpf_xdp_adjust_tail
    bpf_xdp_adjust_meta
    bpf_xdp_redirect
    bpf_xdp_redirect_map
    bpf_skb_get_tunnel_key
    bpf_skb_get_tunnel_opt
    bpf_skb_set_tunnel_key
    bpf_skb_set_tunnel_opt
    bpf_skb_under_cgroup
    bpf_skb_cgroup_id
    bpf_skb_ancestor_cgroup_id
    bpf_get_socket_cookie
    bpf_get_socket_cookie_sock_addr
    bpf_get_socket_cookie_sock_ops
    bpf_get_socket_uid
    bpf_setsockopt
    bpf_getsockopt
    bpf_sock_ops_cb_flags_set
    bpf_bind
    bpf_skb_get_xfrm_state
    bpf_lwt_in_push_encap
    bpf_lwt_xmit_push_encap
    bpf_lwt_seg6_store_bytes
    bpf_lwt_seg6_action
    bpf_lwt_seg6_adjust_srh
    bpf_skc_lookup_tcp
    bpf_sk_lookup_tcp
    bpf_sk_lookup_udp
    bpf_sk_release
    bpf_xdp_sk_lookup_udp
    bpf_xdp_skc_lookup_tcp
    bpf_xdp_sk_lookup_tcp
    bpf_sock_addr_skc_lookup_tcp
    bpf_sock_addr_sk_lookup_tcp
    bpf_sock_addr_sk_lookup_udp
    bpf_get_listener_sock
    bpf_skb_ecn_set_ce
    sk_select_reuseport
    sk_reuseport_load_bytes
    sk_reuseport_load_bytes_relative

.. _helper: https://github.com/iovisor/bpf-docs/blob/master/bpf_helpers.rst
.. _Cilium: https://github.com/cilium/cilium
