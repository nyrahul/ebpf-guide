==========
eBPF Guide
==========

TL;DR
-----

EBPF allows insertion of **bytecode** in Linux kernel at runtime at various
**hookpoints**. On insertion, the bytecode is vetted through a kernel verifier
such that only instruction-set which fulfills certain constraints are allowed.

This tutorial will help you to build a sample ebpf programs and get familiarized with the tool set.

Problem Statements:
~~~~~~~~~~~~~~~~~~~
1. `Block TCP port 8080 <block-tcp-8080.rst>`_
2. `Ack-Thinning <ack-thinning.rst>`_
