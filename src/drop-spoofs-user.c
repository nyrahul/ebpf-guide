/*
 * Notes/TODOs:
 * 1. Supports only IPv4. IPv6 support is easy to add
 * 2. Supports max one address per interface. Ok for IPv4, but with IPv6 an
 *    interface may have multiple addresses.
 * 3. Interface addresses may change dynamically for e.g., based on DHCP refresh. Not handled.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#include <linux/bpf.h>
#include <bpf.h>
#include "drop-spoofs.h"

int g_stat_fd = -1;
int g_iface_map_fd = -1;

void check_stats(void)
{
    unsigned long key = 0, allow_val = 0, drop_val = 0;

    key = ACT_ALLOW;
    bpf_map_lookup_elem(g_stat_fd, &key, &allow_val);

    key = ACT_DROP;
    bpf_map_lookup_elem(g_stat_fd, &key, &drop_val);

    printf("Packets: Allowed %ld and dropped %ld\n",
            allow_val, drop_val);
}

uint32_t get_ipv4_addr_from_ifname(char *ifname)
{
    struct ifaddrs *ifaddr, *ifa;
    uint32_t addr = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(1);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        if (!strcmp(ifa->ifa_name, ifname)) {
            addr = ((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr.s_addr;
            break;
        }
    }

    freeifaddrs(ifaddr);
    return addr;
}

void load_iface_map(void)
{
    struct if_nameindex *if_nidxs, *intf;
    int ret;
    uint32_t addr;

    if_nidxs = if_nameindex();
    if (if_nidxs == NULL) {
        return;
    }

    printf("Loading interface info...\n");
    for (intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++) {
        addr = get_ipv4_addr_from_ifname(intf->if_name);
        if(!addr) continue;

        printf("id=%d name=%s IPv4-addr-in-network-byte-order=0x%x\n",
                intf->if_index, intf->if_name, addr);

        if (intf->if_index > MAX_IFACES) {
            printf("if_index(%d) exceeded MAX_IFACES(%d)\n",
                    intf->if_index, MAX_IFACES);
            exit(1);
        }
        ret = bpf_map_update_elem(g_iface_map_fd, &intf->if_index, &addr, 0);
        if (ret) {
            perror("bpf_map_update_elem");
            exit(1);
        }
    }
    if_freenameindex(if_nidxs);
}

void load_maps(void)
{
    g_stat_fd = bpf_obj_get("/sys/fs/bpf/tc/globals/stat_map");
    if (g_stat_fd < 0) {
        printf("Could not get the stat map\n");
        exit(1);
    }
    g_iface_map_fd = bpf_obj_get("/sys/fs/bpf/tc/globals/iface_map");
    if (g_iface_map_fd < 0) {
        printf("Could not get the iface map\n");
        exit(1);
    }
}

int main(void)
{
    load_maps();
    load_iface_map();
    while (1) {
        check_stats();
        sleep(1);
    }
    return 0;
}

