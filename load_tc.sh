#!/bin/bash

IFACE=lo
[[ $UID -ne 0 ]] && echo "Need to be root" && exit 2

function unload()
{
    tc qdisc del dev $IFACE clsact 2>/dev/null
    tc filter del dev $IFACE egress 2>/dev/null
    umount  /sys/fs/bpf 2>/dev/null
}
function load()
{
    mount -t bpf none /sys/fs/bpf
    tc qdisc add dev $IFACE clsact
    tc filter add dev $IFACE egress bpf da obj bin/drop-spoofs-kern.bo sec egress
    tc filter show dev $IFACE egress
}

unload
[[ "$1" != "" ]] && echo "unloaded" && exit
set -e
load
