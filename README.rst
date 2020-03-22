==========
eBPF Guide
==========

TL;DR
-----

EBPF allows insertion of **bytecode** in Linux kernel at runtime at various
**hookpoints**. On insertion, the bytecode is vetted through a kernel verifier
such that only instruction-set which fulfills certain constraints are allowed.

This tutorial will help you to build a sample ebpf programs and get familiarized with the tool set.

FAQs
~~~~

* `eBPF vs kernel module <docs/ebpf_vs_kernmod.rst>`_
* `GPL license and eBPF <docs/gpl_license_ebpf.rst>`_


Problem Statements:
~~~~~~~~~~~~~~~~~~~

+-------------------------------------------------------------------------+-----------+-----------+
| Problem statement                                                       | Hookpoint | Loader    |
+=========================================================================+===========+===========+
| `Block TCP port 8080 <docs/block-tcp-8080.rst>`_                        | XDP       | iproute2  |
+-------------------------------------------------------------------------+-----------+-----------+
| `Drop spoofed packets from local socks <docs/drop-spoofed-packets.rst>`_| tc-egress | tc        |
+-------------------------------------------------------------------------+-----------+-----------+
| `Localhost Network Bypass <docs/localhost-bypass-stack.rst>`_           | sockops,  | libbpf    |
|                                                                         | strparser |           |
+-------------------------------------------------------------------------+-----------+-----------+

