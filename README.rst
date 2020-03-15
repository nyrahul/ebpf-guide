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

* `eBPF vs kernel module <ebpf_vs_kernmod.rst>`_


Problem Statements:
~~~~~~~~~~~~~~~~~~~
1. `Block TCP port 8080 <docs/block-tcp-8080.rst>`_
2. `Ack-Thinning <docs/ack-thinning.rst>`_
3. `Using Container Context to avoid contain malicious containers <docs/drop-spoofed-packets.rst>`_
4. `Localhost Fast transfer <docs/intrahost-sockmap.rst>`_
