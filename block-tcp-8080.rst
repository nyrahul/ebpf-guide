Problem Statement: Block TCP port 8080 
--------------------------------------
Block TCP port 8080

Step 1: Compile
~~~~~~~~~~~~~~~
Compile ebpf source_ for dropping the packet::

    $ clang -I/usr/include/x86_64-linux-gnu -O2 -target bpf -c src/xdp-drop.c -o xdp_drop.o
        ... [compile ebpf program]

Step 2: Load
~~~~~~~~~~~~
Load ebpf bytecode::

    $ sudo ip link set dev lo xdp obj xdp_drop.o sec .text
        ... [load bytecode using XDP on loopback interface]

Step 3: Verify
~~~~~~~~~~~~~~
Verify the loaded bytecode::

    $ ip link show dev lo
        ... [check the loaded ebpf-xdp code]
        1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 xdpgeneric qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
            link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
            prog/xdp id 58  <------------ NOTE THIS LINE

Step 4: Test
~~~~~~~~~~~~
Test using netcat

In terminal1 start netcat in server mode::

    $ nc -l 8080

In terminal2 start netcat in client mode::

    $ echo hello | nc 127.0.0.1 8080

Outcome
~~~~~~~
If the xdp_drop.o is loaded then the 'hello' should not show up on the server, else it should.

.. _source: src/xdp-drop.c