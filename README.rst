==========
eBPF Guide
==========

TL;DR
-----

EBPF allows insertion of **bytecode** in Linux kernel at runtime at various
**hookpoints**. On insertion, the bytecode is vetted through a kernel verifier
such that only instruction-set which fulfills certain constraints are allowed.

This tutorial will help you to build a sample ebpf programs and get
familiarized with the tool set. For more info, you will find curated list of
ebpf articles `here <https://github.com/zoidbergwill/awesome-ebpf>`_.

Setting up env
--------------
ebpf depends on kernel-headers and in my samples I use libbpf which is shipped
as part of the kernel code. The compilation env depends on kernel source to be
present/compiled successfully.

You can use your own method to get the kernel and compile it. However, this is
what I do (and I usually experiment in a VM env):

1. Go to `kernel.org` and download a **longterm** kernel source/tarball.
2. Compile and install the kernel::

    sudo apt-get install libncurses-dev flex bison openssl libssl-dev \
                 dkms libelf-dev libudev-dev libpci-dev libiberty-dev \
                 autoconf binutils-dev
    cd $SRC #SRC=Kernel base folder
    make defconfig
    make -j $(nproc)
    sudo make modules_install
    sudo make headers_install
    sudo make install

3. Compile bpf related libs

::

    cd $SRC #SRC=Kernel base folder
    make -C samples/bpf
    make -C tools/lib/bpf   #Compiles libbpf.a

4. Compile ebpf guide

``cd ebpf-guide && make KRNDIR=$SRC`` where $SRC is kernel base dir

Note these steps were performed using linux-5.4.39.

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

