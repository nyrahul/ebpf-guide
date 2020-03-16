ifeq (,$(KRNDIR))
KRNDIR = /home/rahul/rnd/linux-5.3
endif

BIN = bin
CL  = clang
CC  = gcc
Q   = @

ifeq ($(V),1)
  Q =
endif

# shamelessly copied from kernel's samples/bpf/Makefile
KF = -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include \
	 -I$(KRNDIR)/arch/x86/include -I$(KRNDIR)/arch/x86/include/generated  \
	 -I$(KRNDIR)/include -I$(KRNDIR)/arch/x86/include/uapi \
	 -I$(KRNDIR)/arch/x86/include/generated/uapi -I$(KRNDIR)/include/uapi \
	 -I$(KRNDIR)/include/generated/uapi \
	 -include $(KRNDIR)/include/linux/kconfig.h \
	 -include asm_goto_workaround.h \
	 -I$(KRNDIR)/samples/bpf -I$(KRNDIR)/tools/testing/selftests/bpf -Isrc \
	 -D__KERNEL__ -D__BPF_TRACING__ -Wno-unused-value -Wno-pointer-sign \
	 -D__TARGET_ARCH_x86 -Wno-compare-distinct-pointer-types \
	 -Wno-gnu-variable-sized-type-not-at-end \
	 -Wno-address-of-packed-member -Wno-tautological-compare \
	 -Wno-unknown-warning-option  \
	 -O2 -emit-llvm

UF  = -Isrc -O2 -I $(KRNDIR)/tools/lib -Wall -I$(KRNDIR)/tools/testing/selftests/bpf
UDF = $(KRNDIR)/tools/lib/bpf/libbpf.a -lelf

SRCDIR=src

SRCS_KERN:=$(wildcard $(SRCDIR)/*-kern.c)
SRCN:=$(notdir $(SRCS_KERN))
BOBJS:=$(patsubst %.c,$(BIN)/%.bo,$(SRCN))

SRCS_USER:=$(wildcard $(SRCDIR)/*-user.c)
SRCN:=$(notdir $(SRCS_USER))
UBINS:=$(patsubst %.c,$(BIN)/%.bin,$(SRCN))

UTIL_SRC=$(KRNDIR)/tools/testing/selftests/bpf/cgroup_helpers.c

vpath %.c $(SRCDIR)

RED=\033[0;31m
GREEN=\033[0;32m
CYAN=\033[0;36m
NC=\033[0m

.PHONY: all
all: chkdir $(BOBJS) $(UBINS)

.PHONY: chkdir
chkdir:
ifeq (,$(wildcard $(KRNDIR)/Kconfig))
	@echo "Your kernel path[$(RED)$(KRNDIR)$(NC)] is incorrect. Use 'make KRNDIR=[KERNEL-SRC-PATH]'."
	Quitting abnormally
endif
	@mkdir -p $(BIN) 2>/dev/null

$(BIN)/%.bin: %.c
	@echo "Compiling user-space app: $(CYAN)$@$(NC) ..."
	$(Q)$(CC) $(UF) $(UTIL_SRC) $< -o $@ $(UDF)

$(BIN)/%.bo: %.c
	@echo "Compiling eBPF bytecode: $(GREEN)$@$(NC) ..."
	$(Q)$(CL) $(KF) -c $< -o -| llc -march=bpf -mcpu=probe -filetype=obj -o $@

.PHONY: clean
clean:
	@rm -rf $(BIN)

