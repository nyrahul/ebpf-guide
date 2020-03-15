#!/bin/bash
[[ $UID -ne 0 ]]             && echo "need to be root"   && exit 2
[[ $(which curl) == "" ]]    && echo "need util curl"    && exit 2
[[ $(which timeout) == "" ]] && echo "need util timeout" && exit 2

CGRP_MNT=/tmp/cgroupv2
FOO=$CGRP_MNT/foo
PINPT=/sys/fs/bpf/mytestprog
BO=bin/local-socket-bypass-kern.bo
BPF_MNT=/sys/fs/bpf

trap 'unload "$BASH_COMMAND"' EXIT

unload()
{
    [[ $? -ne 0 ]] && echo "Failed: [$1]"
    echo "unloading..."
    set +e
    bpftool cgroup detach $FOO sock_ops pinned $PINPT 2>/dev/null
    umount $CGRP_MNT
    [[ -d "$CGRP_MNT" ]] && rm -rf $CGRP_MNT
    umount $BPF_MNT
}

# Inspired by: https://github.com/torvalds/linux/blob/master/samples/bpf/tcp_bpf.readme
load()
{
    mountpoint $BPF_MNT >/dev/null
    [[ $? -ne 0 ]] && mount -t bpf none $BPF_MNT

    mkdir -p $CGRP_MNT
    mountpoint $CGRP_MNT >/dev/null
    [[ $? -ne 0 ]] && mount -t cgroup2 none $CGRP_MNT

    set -eE -o functrace
    mkdir -p $FOO 
    echo $$ >> $FOO/cgroup.procs
    bpftool prog load $BO $PINPT
    bpftool cgroup attach $FOO sock_ops pinned $PINPT
}

load

### Start of Action

curl http://www.baidu.com

### End of Action

timeout --preserve-status 1s bpftool prog tracelog
