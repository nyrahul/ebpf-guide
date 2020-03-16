/*
 * Bypass network stack for local sockets
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/bpf.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include <cgroup_helpers.h>

#define FATAL(...) { printf(__VA_ARGS__); exit(1); }
#define ERROR(...) { printf(__VA_ARGS__); }
#define INFO(...)  { printf(__VA_ARGS__); }

#if 0
void load_prog(void)
{
    int err, parse_prog, verdict_prog, sockops_prog;
    struct bpf_object *obj;
    struct bpf_map *skmap;

    err = bpf_prog_load("bin/local-socket-bypass-sockops-kern.bo",
                 BPF_PROG_TYPE_SOCK_OPS, &obj, &sockops_prog);
    if (err) FATAL("sockops bpf_prog_load failed err:%d\n", err);

    skmap = bpf_object__find_map_by_name(obj, "skmap");
    if (!skmap) FATAL("cud not find skmap\n");

    g_skmap_fd = bpf_map__fd(skmap);
    if (g_skmap_fd < 0) FATAL("Could not get the stat map\n");

    /* Load parser, get skmap fd and attach fd to parser */
    err = bpf_prog_load("bin/local-socket-bypass-parse-kern.bo",
                 BPF_PROG_TYPE_SK_SKB, &obj, &parse_prog);
    if (err) FATAL("parse bpf_prog_load failed err:%d\n", err);

    err = bpf_prog_attach(parse_prog, g_skmap_fd, BPF_SK_SKB_STREAM_PARSER, 0);
    if (err) FATAL("bpf_prog_attach failed err=%d\n", err);

    /* Load verdict, skmap fd we already have and attach fd to verdict */
    err = bpf_prog_load("bin/local-socket-bypass-verdict-kern.bo",
                 BPF_PROG_TYPE_SK_SKB, &obj, &verdict_prog);
    if (err) FATAL("verdict bpf_prog_load failed err:%d\n", err);

    err = bpf_prog_attach(verdict_prog, g_skmap_fd, BPF_SK_SKB_STREAM_VERDICT, 0);
    if (err) FATAL("bpf_prog_attach failed err=%d\n", err);
}
#endif

int prog_attach_type[] = {
    BPF_SK_SKB_STREAM_PARSER,
    BPF_SK_SKB_STREAM_VERDICT,
    BPF_CGROUP_SOCK_OPS,
};

int prog_type[] = {
    BPF_PROG_TYPE_SK_SKB,
    BPF_PROG_TYPE_SK_SKB,
    BPF_PROG_TYPE_SOCK_OPS,
};

int prog_fd[3];
int cgrp_fd = -1;
int g_skmap_fd = -1;

#define CGRP_PATH "/bypass"

void load_prog(void)
{
    struct bpf_program *prog;
    struct bpf_object *obj;
    struct bpf_map *skmap;
    long   err;
    int    ret;
    int    i = 0;

    obj = bpf_object__open("bin/local-socket-bypass-sockops-kern.bo");
    err = libbpf_get_error(obj);
    if (err) {
        char err_buf[256];

        libbpf_strerror(err, err_buf, sizeof(err_buf));
        FATAL("object open failed [%s]\n", err_buf);
    }
    
    bpf_object__for_each_program(prog, obj) {
        bpf_program__set_type(prog, prog_type[i]);
        bpf_program__set_expected_attach_type(prog, prog_attach_type[i]);
        i++;
    }

    i = bpf_object__load(obj);
    i = 0;
    bpf_object__for_each_program(prog, obj) {
        prog_fd[i] = bpf_program__fd(prog);
        i++;
    }

    skmap      = bpf_object__find_map_by_name(obj, "skmap");
    g_skmap_fd = bpf_map__fd(skmap);
    if (g_skmap_fd < 0) FATAL("cud not find map\n");

    ret = bpf_prog_attach(prog_fd[0], g_skmap_fd,
                    BPF_SK_SKB_STREAM_PARSER, 0);
    if (ret) FATAL("attach sockmap to parser failed\n");

    ret = bpf_prog_attach(prog_fd[1], g_skmap_fd,
                    BPF_SK_SKB_STREAM_VERDICT, 0);
    if (ret) FATAL("attach sockmap to verdict failed\n");

    ret = bpf_prog_attach(prog_fd[2], cgrp_fd, BPF_CGROUP_SOCK_OPS, 0);
    if (ret) FATAL("attach sockops to cgrp failed\n");

}

int main(int argc, char *argv[])
{
    int ret;

    if (argc < 2) FATAL("Usage: %s <script-to-exec>\n", argv[0]);

    if (setup_cgroup_environment()) FATAL("setup cgrp failed\n");
    cgrp_fd = create_and_get_cgroup(CGRP_PATH);
    if (cgrp_fd < 0) FATAL("create cgrp failed\n");
    if (join_cgroup(CGRP_PATH)) FATAL("join cgrp failed\n");

    load_prog();
    ret = system(argv[1]);
    INFO("%s ret=%d\n", argv[1], ret);

    cleanup_cgroup_environment();
    close(cgrp_fd);

    return 0;
}

