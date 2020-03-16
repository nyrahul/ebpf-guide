#!/bin/bash

chktool()
{
    [[ $(which "$1") == "" ]] && echo "need util $1" && exit 2
}

chktool curl
chktool python
chktool timeout
chktool bpftool
chktool mountpoint

python -m SimpleHTTPServer 12345 &
srv_pid=$!
sleep 1
curl http://localhost:12345/
kill $srv_pid

timeout --preserve-status 1s bpftool prog tracelog #show whatever logs were captured
