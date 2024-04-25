#pragma once

#include <linux/types.h>
#include "config.h"

struct cmd_line
{
    const char *config;
    unsigned int list : 1;
    unsigned int verbose : 1;
    unsigned int help : 1;
    unsigned int cli : 1;

    /* Sequence options. */
    char *interface;

    unsigned int block : 1;
    unsigned int track : 1;
    unsigned int l4_csum : 1;

    __u64 max_pckts;
    __u64 max_bytes;

    __u64 pps;
    __u64 bps;

    __u64 time;
    __u64 delay;
    
    __u16 threads;
    
    char *src_mac;
    char *dst_mac;

    __u16 ttl_min;
    __u16 ttl_max;
    __u16 id_min;
    __u16 id_max;
    char *src_ip;
    char *dst_ip;
    char *protocol;
    __u8 tos;
    unsigned int l3_csum : 1;

    __u16 udp_src_port;
    __u16 udp_dst_port;

    __u16 tcp_src_port;
    __u16 tcp_dst_port;
    unsigned int tcp_syn : 1;
    unsigned int tcp_ack : 1;
    unsigned int tcp_psh : 1;
    unsigned int tcp_rst : 1;
    unsigned int tcp_fin : 1;
    unsigned int tcp_urg : 1;
    unsigned int tcp_ece : 1;
    unsigned int tcp_cwr : 1;
    unsigned int tcp_cooked : 1;
    unsigned int tcp_one_connection : 1;

    __u8 icmp_code : 1;
    __u8 icmp_type : 1;

    __u16 pl_min_len;
    __u16 pl_max_len;
    unsigned int pl_is_static : 1;
    char *pl_exact;
    unsigned int pl_is_file : 1;
    unsigned int pl_is_string : 1;
};

void print_cmd_help();
void parse_cmd_line(int argc, char *argv[], struct cmd_line *cmd);
void parse_cli(struct cmd_line *cmd, struct config *cfg);