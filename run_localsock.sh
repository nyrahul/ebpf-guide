#!/bin/bash

# This is called from src/local-socket-bypass-user.c

trap 'cleanup' EXIT

cleanup()
{
    [[ "$srv_pid" != "" ]] && kill $srv_pid
}

workload1()
{
    python -m SimpleHTTPServer 12345 &
    srv_pid=$!
    sleep 1
    curl http://localhost:12345/
    #curl http://www.ietf.org/
}

workload2()
{
    echo "TOCLIXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" | nc -l 127.0.0.1 12345 &
    srv_pid=$!
    sleep 0.5
    i=1
    while [ $i -le 20 ]; do
        echo "TOSRV$i"
        sleep 0.2
        ((i++))
    done | nc -w 1 127.0.0.1 12345
}

workload2

timeout --preserve-status 1s tools/bpftool prog tracelog #show whatever logs were captured
