How does eBPF code compares with Kernel Module?
-----------------------------------------------

Both eBPF and kernel modules execute their code in the kernel-space and in some cases either of them could be used to satisfy the use-case. Depending on your vantage point, it is arguably true that comparing kernel modules to eBPF may not be fair. Nevertheless this question comes up time and again and this article highlights the differences.

Safety, Risk Profile
********************
*eBPF has safety guarantees, kernel modules don't*.

eBPF bytecode is vetted by kernel verifier i.e., the verifier checks whether there are no unbounded loops, improper memory access and only allows certain instruction set. There is no such constraint on what kernel module can do. Thus kernel modules could potentially crash the kernels while eBPF (mostly) won't. Thus eBPF can be readily used in production environments with much less risk. Imagine a situation where you see a problem in your production environment and you need to debug it with least risk and least performance impact. This is a typical eBPF tracing use-case. 

Constraints
***********
*Kernel modules are much more flexible in instruction set than eBPF*.

Use of kernel verifier provides eBPF with safety guarantees but at the same time it imposes constraints [1]_ on what it can allow from eBPF bytecode. Verifier needs to run in limited time and with limited resources hence it imposes constraints on what bytecode can do.

Preemption
**********
*eBPF instruction-set execution cannot be preempted by kernel*.

Kernel module follows regular kernel code preemption logic.

Global/Shared Data
******************
*eBPF cannot access global data unlike kernel modules*.

Since the eBPF instruction set execution is non-preemptible you can store the shared data in PRECPU_MAP.

Reusing existing lib
********************
*eBPF does not allow linking up of execution logic into functions outside of eBPF programs, for e.g., shared libs*.

Kernel modules are flexible to split up the code into functions and they can use external helper libs. Reusing eBPF functions in different bytecodes is possible through tail calls and is much more challenging as compared to kernel modules. 

Satisfying Tracing use-case
***************************
*Kernel modules cannot be used for tracing, eBPF's most widely used scenario is tracing*.

Kernel modules have a specific entry point (init_module()) and exit (cleanup_module()) point. eBPF can be hooked to any kprobe/kretprobe/tracepoint and thus can be used for tracing i.e., to check if the function in the kernel was called and even to check with what parameter values. 

Logging
*******
*Kernel modules can use rich logging system provided by kernel log primitives, eBPF cannot use those*.

.. [1] https://www.kernel.org/doc/html/latest/bpf/bpf_design_QA.html#q-what-are-the-verifier-limits