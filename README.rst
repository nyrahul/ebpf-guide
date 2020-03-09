==========
eBPF Guide
==========

EBPF allows insertion of **bytecode** in Linux kernel at runtime at various
**hookpoints**. On insertion, the bytecode is vetted through a kernel verifier
such that only instruction-set which fulfills certain constraints is allowed.
Constraints are:

1. no loops in instruction-set
2. no access to out-of-kernel memory
3. upper limit on program code size
4. upper limit on stack size

and many more.

This tutorial will help you to build a sample ebpf program and get your familiarized with the tooling.

Problem Statement
-----------------
Some time ago, I worked on a project related to TCP Ack thinning. Aim was to check the impact of ACK traffic reduction on TCP performance under different conditions. Long story short, one of the aspect was to analyse which all conditions are the ACKs getting generated in TCP stack.

The task is to identify all the places in linux kernel from where TCP Ack is getting generated. A typical kernel tracing use-case for which ebpf is suited. 
