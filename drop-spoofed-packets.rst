Problem Statement: Drop Spoofed Packets 
---------------------------------------
With applications running on containers and a single host running multiple
containers, it becomes very easy for a malicious container to spoof packets on
behalf of other container. Unless explicit provisions are made the host kernel
has no way of differentiating such spoofed packets.

However it is possible to make use of the knowledge that every interface has
unique interface ID and for the packets originated from the container should
always have the source address of that container. It is possible to verify
whether the ifindex from which the packets is originated has the right source
address. If the source address does not match then the packet can be considered
spoofed and dropped.

Solution details
----------------
1. `User space app <src/drop-spoofs-user.c>`_: Responsible to load a map containing the ifindex and the corresponding IPv4 address. This code also shows runtime stats of allowed/dropped packets.
2. `Kernel space ebpf <src/drop-spoofs-kern.c>`_: Responsible to do run-time checks on the packets based on the map populated from user-space app. Also upgate the runtime stats about dropped, allowed packets.
3. `Script to load ebpf <load_tc.sh>`_: Use ``tc egress`` point to load the ebpf bytecode in context to loopback interface.
4. `Script to spoof packets <spoof-pkt.py>`_: Scapy based python script to spoof UDP packets.

How to test?
------------
The `default script <load_tc.sh>`_ loads the ebpf bytecode in context to ``lo``
interface. The ``lo`` by default uses the ip address of ``127.0.0.1``. The
`spoof packet script <spoof-pkt.py>`_ sends UDP packets with source (malicious)
IP ``157.0.0.10`` to destination IP ``127.0.0.1``. Here since the source IP
address of ``lo`` interface does not match the packets should be dropped.
However if you do ``ping 127.0.0.1`` from the command line, those packets
should go through. You can monitor the change in allowed/dropped packets at
runtime using ``sudo ./bin/drop-spoofs-user.bin`` command.
