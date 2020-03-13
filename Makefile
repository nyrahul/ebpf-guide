BIN=bin
KRNDIR=/home/rahul/rnd/linux-5.3
CL=clang
CC=gcc
CFLAGS:=-Isrc -O2 -Wall
KCFLAGS:=$(CFLAGS) -funroll-loops -I/usr/include/x86_64-linux-gnu -O2 -target bpf
UCFLAGS:=$(CFLAGS) -I $(KRNDIR)/tools/lib -Wall
ULDFLAGS:=$(KRNDIR)/tools/lib/bpf/libbpf.a -lelf
SRCDIR=src

SRCS_KERN:=$(wildcard $(SRCDIR)/*-kern.c)
SRCN:=$(notdir $(SRCS_KERN))
BOBJS:=$(patsubst %.c,$(BIN)/%.bo,$(SRCN))

SRCS_USER:=$(wildcard $(SRCDIR)/*-user.c)
SRCN:=$(notdir $(SRCS_USER))
UBINS:=$(patsubst %.c,$(BIN)/%.bin,$(SRCN))

vpath %.c $(SRCDIR)

.PHONY: all
all: mkdir $(BOBJS) $(UBINS)

.PHONY: mkdir
mkdir:
	@mkdir -p $(BIN) 2>/dev/null

$(BIN)/%.bin: %.c
	$(CC) $(UCFLAGS) $< -o $@ $(ULDFLAGS)

$(BIN)/%.bo: %.c
	$(CL) $(KCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BIN)
