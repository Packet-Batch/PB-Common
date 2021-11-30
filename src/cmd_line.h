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
    char *cl_interface;
    unsigned int cl_block : 1;
    __u64 cl_count;
    __u64 cl_time;
    __u64 cl_delay;
    __u64 cl_max_data;
    unsigned int cl_track_count : 1;
    __u16 cl_threads;
    unsigned int cl_l4_csum : 1;

    char *cl_src_mac;
    char *cl_dst_mac;

    __u16 cl_ttl_min;
    __u16 cl_ttl_max;
    __u16 cl_id_min;
    __u16 cl_id_max;
    char *cl_src_ip;
    char *cl_dst_ip;
    char *cl_protocol;
    __u8 cl_tos;
    unsigned int cl_l3_csum : 1;

    __u16 cl_udp_src_port;
    __u16 cl_udp_dst_port;

    __u16 cl_tcp_src_port;
    __u16 cl_tcp_dst_port;
    unsigned int cl_tcp_syn : 1;
    unsigned int cl_tcp_ack : 1;
    unsigned int cl_tcp_psh : 1;
    unsigned int cl_tcp_rst : 1;
    unsigned int cl_tcp_fin : 1;
    unsigned int cl_tcp_urg : 1;
    unsigned int cl_tcp_use_socket : 1;

    __u8 cl_icmp_code : 1;
    __u8 cl_icmp_type : 1;

    __u16 cl_pl_min_len;
    __u16 cl_pl_max_len;
    unsigned int cl_pl_is_static : 1;
    char *cl_pl_exact;
    unsigned int cl_pl_is_file : 1;
    unsigned int cl_pl_is_string : 1;
};

void parse_cmd_line(int argc, char *argv[], struct cmd_line *cmd);
void parse_cli(struct cmd_line *cmd, struct config *cfg);