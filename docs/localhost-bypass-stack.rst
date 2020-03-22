Problem Statement: Bypass Network Stack for local TCP sockets
=============================================================

What is it?
-----------
A TCP socket will go through all the phases of TCP/IP packet processing even
when the remote peer is a local TCP socket. The aim of this ebpf code is to
bypass this network stack processing for TCP client/server communication on
local sockets.  The sockmap_ primitive added to ebpf allows to parse/redirect
the traffic between the sockets.

Why would anyone use TCP sockets locally?
---------------------------------------------
* Apps use TCP sockets for inter-process comm since their processes may be
  local or remote. By using TCP, their code works whether the other process is
  on the same machine or another. E.g., ``git`` command syncs locally with
  ``git-credential-cache--daemon`` using local TCP socket.
* Many use TCP sockets for inter-process comm just because of portability. TCP
  sockets are available across any platform. The same is not true for message
  queues, unix domain socks etc.
* Sidecar pattern used in Containers: Sidecars are utility containers who aid
  the main container, typically by automating discovery, logging, OAM, and even
  enforcing security (for e.g., converting non-TLS conns to TLS) on behalf of
  primary/main container. This requires that in some cases (the TLS case
  mentioned) the traffic is proxied through the sidecar.

Why to bypass network stack?
--------------------------------
* Mainly for performance reason. In a typical container env, a packet
  may be tossed locally between different sockets before the data finally
  leaves the node or before it gets consumed somewhere locally. Using TCP
  sockets means this tossing has severe overhead, in terms of, TCP/IP header
  encap/decap, fib lookup, TCP congestion/flow control, transport qdisc
  handling and other interface related operations.
* sockmap_ integrates with kernel's strparser_ and allows one to parse, process
  update the payload before redirecting. Check: ktls-bpf_.

Side-effects of bypassing?
------------------------------
* Possibly. If you have tooling which depends on this traffic passing through
  network stack then yes. For e.g., you may have an iptables rule to do xyz.
  This xyz would not work anymore, since the data is directly tossed between
  the sockets.

Solution details: How eBPF helps?
=================================

Use of sockmap
--------------
- Special SOCKMAP map
- Ways to initialize this sockmap from userspace or kernelspace
- Redirecting packets
    - Use of BPF_F_INGRESS flag
- Parser and verdict eBPF functions
Draw a picture to specify this

What are the pieces?
--------------------
1. `Kernel space ebpf <src/local-socket-bypass-kern.c>`_: TODO
2. `Script to load ebpf <./run_as_cgroupv2.sh>`_
3. `Script to load local app <./run_localsock.py>`_

How to test?
------------
TODO

Read More
---------
* `sockmap commit <https://lwn.net/Articles/731133/>`_
* `Cloudflare blog <https://blog.cloudflare.com/sockmap-tcp-splicing-of-the-future/>`_
* strparse_

.. strparser: https://www.kernel.org/doc/Documentation/networking/strparser.txt
.. sockmap: https://lwn.net/Articles/731133/
.. ktls-bpf: http://vger.kernel.org/lpc_net2018_talks/ktls_bpf_paper.pdf
