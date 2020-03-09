Problem Statement - Ack Thinning
--------------------------------
Some time ago, I worked on a project related to TCP Ack thinning. Aim was to check the impact of ACK traffic reduction on TCP performance under different conditions.

Long story short, one of the aspect was to analyse under which conditions are the ACKs getting generated in the kernel TCP stack.

A typical tracing use-case for which ebpf is suited.

Step 1: Environment Preparation
-------------------------------

1. Get linux kernel code (I am using v5.3)::

    $ git clone -b 'v5.3' --depth 1 https://github.com/torvalds/linux
        ... [~185 MB download]
    $ make defconfig
        ... [This by default enables all the config options required.]
    $ make -j $(nproc)
        ... [patience, this may take few mins to few hours dep on your beast]
    $ make headers_install
    $ make -C samples/bpf
        ... [This may spit errors if dep tools such as clang/llvm are not installed. Just follow the errors.]

2. Change threshold for ACKs in the kernel
3. Monitor Acks using bpftrace
4. xxx
