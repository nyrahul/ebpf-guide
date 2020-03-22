#!/bin/bash

chktool()
{
    [[ $(which "$1") == "" ]] && echo "need util $1" && exit 2
}

[[ $UID -ne 0 ]] && echo "need to be root" && exit 2

chktool curl
chktool python
chktool timeout
chktool bpftool
chktool mountpoint

CGRP_MNT=/mnt/cgroup-test-work-dir
BPF_MNT=/sys/fs/bpf

trap 'unload "$BASH_COMMAND"' EXIT

unload()
{
    [[ $? -ne 0 ]] && echo "Failed: [$1]"
    echo "unloading..."
    set +e

    if [ -d "$CGRP_MNT" ]; then
        mountpoint $CGRP_MNT >/dev/null
        [[ $? -eq 0 ]] && umount $CGRP_MNT
        [[ -d "$CGRP_MNT" ]] && rm -rf $CGRP_MNT
    fi

    mountpoint $BPF_MNT >/dev/null
    [[ $? -eq 0 ]] && umount $BPF_MNT
}

load()
{
    mountpoint $BPF_MNT >/dev/null
    [[ $? -ne 0 ]] && mount -t bpf none $BPF_MNT

    mkdir -p $CGRP_MNT
    mountpoint $CGRP_MNT >/dev/null
    [[ $? -ne 0 ]] && mount -t cgroup2 none $CGRP_MNT
}

#load

./bin/local-socket-bypass-user.bin ./run_localsock.sh

### Start of Action

#python -m SimpleHTTPServer 12345 &
#srv_pid=$!
#sleep 1
#curl http://localhost:12345/
#curl http://www.baidu.com
#kill $srv_pid $bypass_pid

### End of Action

#timeout --preserve-status 1s bpftool prog tracelog #show whatever logs were captured
