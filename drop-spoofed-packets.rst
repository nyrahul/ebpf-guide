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
1. `User space app <src/drop-spoofs-user.c>`: Responsible to load a map containing the ifindex and the corresponding IPv4 address. This code also shows runtime stats of allowed/dropped packets.
2. `Kernel space ebpf <src/drop-spoofs-kern.c>`: Responsible to do run-time checks on the packets based on the map populated from user-space app. Also upgate the runtime stats about dropped, allowed packets.
3. `Script to load ebpf <load_tc.sh>`: Use ``tc egress`` point to load the ebpf bytecode.
4. `Script to spoof packets <spoof-pkt.py>`: Scapy based python script to spoof UDP packets.

