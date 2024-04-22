#pragma once

#include <linux/types.h>
#include <linux/if_ether.h>

#define MAX_INCLUDES 64
#define MAX_SEQUENCES 256
#define MAX_RANGES 64

#define MAX_PAYLOADS 256

struct eth_opt
{
    char *src_mac;
    char *dst_mac;
};

struct ip_opt
{
    // Protocol (Required).
    char *protocol;

    // Source and destination addresses (Required).
    char *src_ip;
    char *dst_ip;
    char *ranges[MAX_RANGES];
    __u16 range_count;

    // Type of Service.
    __u8 tos;

    // Time to Live.
    __u8 min_ttl;
    __u8 max_ttl;

    // ID.
    __u16 min_id;
    __u16 max_id;

    // Do checksum.
    unsigned int csum : 1;
};

struct tcp_opt
{
    __u16 src_port;
    __u16 dst_port;

    // TCP flags.
    unsigned int syn : 1;
    unsigned int psh : 1;
    unsigned int fin : 1;
    unsigned int ack : 1;
    unsigned int rst : 1;
    unsigned int urg : 1;
    unsigned int ece : 1;
    unsigned int cwr : 1;

    unsigned int cooked : 1;
    unsigned int one_connection : 1;
};

struct udp_opt
{
    __u16 src_port;
    __u16 dst_port;
};

struct icmp_opt
{
    __u8 code;
    __u8 type;
};

struct payload_opt
{
    __u16 min_len;
    __u16 max_len;
    unsigned int is_static : 1;
    unsigned int is_file : 1;
    unsigned int is_string : 1;
    char *exact;
};

struct sequence
{
    // General options.
    char *interface;
    unsigned int block : 1;
    __u64 max_count;
    __u64 time;
    __u64 delay;
    __u64 max_data;
    __u16 threads;
    char *includes[MAX_INCLUDES];
    __u16 include_count;
    unsigned int track_count : 1;

    // Ethernet options.
    struct eth_opt eth;

    // IP options.
    struct ip_opt ip;

    // Layer 4 options.
    struct tcp_opt tcp;
    struct udp_opt udp;
    struct icmp_opt icmp;
    unsigned int l4_csum : 1;

    // Payload options.
    int pl_cnt;
    struct payload_opt pls[MAX_PAYLOADS];
};

struct config
{
    // Device options.
    char *interface;

    struct sequence seq[MAX_SEQUENCES];
};

int parse_config(const char file_name[], struct config *cfg, int only_seq, int *seq_num, __u8 log);
void clear_sequence(struct config *cfg, int seq_num);
void print_config(struct config *cfg, int seq_cnt);