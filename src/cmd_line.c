#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "cmd_line.h"
#include "config.h"

static struct option long_opts[] =
{
    {"cfg", required_argument, NULL, 'c'},
    {"cli", no_argument, NULL, 'z'},
    {"list", no_argument, NULL, 'l'},
    {"verbose", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},

    /* CLI options. */
    {"interface", required_argument, NULL, 0},
    {"send", required_argument, NULL, 1},
    {"block", required_argument, NULL, 2},
    {"count", required_argument, NULL, 3},
    {"time", required_argument, NULL, 4},
    {"delay", required_argument, NULL, 5},
    {"trackcount", required_argument, NULL, 6},
    {"data", required_argument, NULL, 7},
    {"threads", required_argument, NULL, 8},
    {"l4csum", required_argument, NULL, 9},

    {"smac", required_argument, NULL, 10},
    {"dmac", required_argument, NULL, 11},

    {"minttl", required_argument, NULL, 12},
    {"maxttl", required_argument, NULL, 13},
    {"minid", required_argument, NULL, 14},
    {"maxid", required_argument, NULL, 15},
    {"sip", required_argument, NULL, 16},
    {"dip", required_argument, NULL, 17},
    {"protocol", required_argument, NULL, 18},
    {"tos", required_argument, NULL, 19},
    {"l3csum", required_argument, NULL, 20},

    {"usport", required_argument, NULL, 21},
    {"udport", required_argument, NULL, 22},

    {"tsport", required_argument, NULL, 23},
    {"tdport", required_argument, NULL, 24},
    {"syn", required_argument, NULL, 25},
    {"ack", required_argument, NULL, 26},
    {"psh", required_argument, NULL, 27},
    {"rst", required_argument, NULL, 28},
    {"fin", required_argument, NULL, 29},
    {"urg", required_argument, NULL, 30},
    {"ece", required_argument, NULL, 41},
    {"cwr", required_argument, NULL, 42},
    {"usesocket", required_argument, NULL, 31},
    {"oneconnection", required_argument, NULL, 40},

    {"icode", required_argument, NULL, 38},
    {"itype", required_argument, NULL, 39},

    {"pmin", required_argument, NULL, 32},
    {"pmax", required_argument, NULL, 33},
    {"pstatic", required_argument, NULL, 34},
    {"pexact", required_argument, NULL, 35},
    {"pfile", required_argument, NULL, 36},
    {"pstring", required_argument, NULL, 37},

    {NULL, 0, NULL, 0}
};

/**
 * Prints the command line help menu.
 * 
 * @return void
**/
void print_cmd_help()
{
    fprintf(stdout, "Usage: pcktbatch -c <configfile> [-v -h ...]\n");

    // Basic.
    fprintf(stdout, "Basic\n");
    fprintf(stdout, "\t-c --cfg => Path to config file.");
    fprintf(stdout, "\t-l --list => Print full config values.\n");
    fprintf(stdout, "\t-v --verbose => Provide verbose output on packets sent.\n");
    fprintf(stdout, "\t-h --help => Print out the help menu and exit.\n");

    // First sequence override.
    fprintf(stdout, "First Sequence/Packet Override\n");
    fprintf(stdout, "\t-z --cli => Enables the first sequence/packet override.\n\n");

    fprintf(stdout, "\t--interface => The interface to send out of.");
    fprintf(stdout, "\t--block => Whether to enable blocking mode (0/1).");
    fprintf(stdout, "\t--count => The maximum amount of packets to send during this sequence before exiting.\n");
    fprintf(stdout, "\t--delay => The delay in-between sending packets on each thread.\n");
    fprintf(stdout, "\t--data => The maximum amount of data (in bytes) we can send during this sequence before exiting.\n");
    fprintf(stdout, "\t--trackcount => Keep track of count regardless of it being 0 (read Configuration explanation for more information) (0/1).\n");
    fprintf(stdout, "\t--threads => The amount of threads and sockets to spawn (0 = CPU count).\n");
    fprintf(stdout, "\t--l4csum => Whether to calculate the layer-4 checksum (TCP, UDP, and ICMP) (0/1).\n\n");

    fprintf(stdout, "\t--smac => The ethernet source MAC address to use.\n");
    fprintf(stdout, "\t--dmac => The ethernet destination MAC address to use.\n\n");

    fprintf(stdout, "\t--minttl => The minimum IP TTL to use.\n");
    fprintf(stdout, "\t--maxttl => The maximum IP TTL to use.\n");
    fprintf(stdout, "\t--minid => The minimum IP ID to use.\n");
    fprintf(stdout, "\t--maxid => The maximum IP ID to use.\n");
    fprintf(stdout, "\t--sip => The source IP (one range is supported in CIDR format).\n");
    fprintf(stdout, "\t--dip => The destination IP.\n");
    fprintf(stdout, "\t--protocol => The protocol to use (TCP, UDP, or ICMP).\n");
    fprintf(stdout, "\t--tos => The IP TOS to use.\n");
    fprintf(stdout, "\t--l3csum => Whether to calculate the IP header checksum or not (0/1).\n\n");

    fprintf(stdout, "\t--usport => The UDP source port.\n");
    fprintf(stdout, "\t--udport => The UDP destination port.\n\n");

    fprintf(stdout, "\t--cooked => Use cooked TCP sockets which establishes a connection automatically.\n");
    fprintf(stdout, "\t--oneconnection => Send packets over the same socket/connection instead of making a new connection for each packet.\n");
    fprintf(stdout, "\t--tsport => The TCP source port.\n");
    fprintf(stdout, "\t--tdport => The TCP source port.\n");
    fprintf(stdout, "\t--syn => Set the TCP SYN flag (0/1).\n");
    fprintf(stdout, "\t--ack => Set the TCP ACK flag (0/1).\n");
    fprintf(stdout, "\t--psh => Set the TCP PSH flag (0/1).\n");
    fprintf(stdout, "\t--rst => Set the TCP RST flag (0/1).\n");
    fprintf(stdout, "\t--fin => Set the TCP FIN flag (0/1).\n");
    fprintf(stdout, "\t--urg => Set the TCP URG flag (0/1).\n");
    fprintf(stdout, "\t--ece => Set the TCP ECE flag (0/1).\n");
    fprintf(stdout, "\t--cwr => Set the TCP CWR flag (0/1).\n\n");

    fprintf(stdout, "\t--pmin => The minimum payload data.\n");
    fprintf(stdout, "\t--pmax => The maximum payload data.\n");
    fprintf(stdout, "\t--pstatic => Use static payload (0/1).\n");
    fprintf(stdout, "\t--pexact => The exact payload string.\n");
    fprintf(stdout, "\t--pfile => Whether to parse a file as the 'pexact' string instead.\n");
    fprintf(stdout, "\t--pstring => Parse the 'pexact' string or file as a string instead of hexadecimal.\n");
}

/**
 * Parses CLI options if --cli is passed.
 * 
 * @param cmd The cmd_line structure to grab the command line values from.
 * @param cfg The config structure to save the command line values to.
 * @param sequence
 * 
 * @return void
**/
void parse_cli(struct cmd_line *cmd, struct config *cfg)
{
    struct sequence *seq = &cfg->seq[0];

    if (cmd->cl_interface != NULL)
    {
        cfg->interface = cmd->cl_interface;
        seq->interface = cmd->cl_interface;
    }

    seq->block = cmd->cl_block;
    seq->max_count = cmd->cl_count;
    seq->time = cmd->cl_time;
    seq->delay = cmd->cl_delay;
    seq->track_count = cmd->cl_track_count;
    seq->max_data = cmd->cl_max_data;
    seq->threads = cmd->cl_threads;

    seq->l4_csum = cmd->cl_l4_csum;

    seq->eth.src_mac = cmd->cl_src_mac;
    seq->eth.dst_mac = cmd->cl_dst_mac;

    seq->ip.min_ttl = cmd->cl_ttl_min;
    seq->ip.max_ttl = cmd->cl_ttl_max;
    seq->ip.min_id = cmd->cl_id_min;
    seq->ip.max_id = cmd->cl_id_max;

    if (cmd->cl_src_ip != NULL)
    {
        // Check for range.
        if (strstr(cmd->cl_src_ip, "/") != NULL)
        {
            seq->ip.src_ip = 0;
            seq->ip.range_count = 1;
            seq->ip.ranges[0] = cmd->cl_src_ip;
        }
        else
        {
            seq->ip.src_ip = cmd->cl_src_ip;
        }
    }

    if (cmd->cl_dst_ip != NULL)
    {
        seq->ip.dst_ip = cmd->cl_dst_ip;
    }

    seq->ip.protocol = cmd->cl_protocol;
    seq->ip.tos = cmd->cl_tos;
    seq->ip.csum = cmd->cl_l3_csum;

    seq->udp.src_port = cmd->cl_udp_src_port;
    seq->udp.dst_port = cmd->cl_udp_dst_port;

    seq->tcp.cooked = cmd->cl_tcp_cooked;
    seq->tcp.one_connection = cmd->cl_tcp_one_connection;
    seq->tcp.src_port = cmd->cl_tcp_src_port;
    seq->tcp.dst_port = cmd->cl_tcp_dst_port;
    seq->tcp.syn = cmd->cl_tcp_syn;
    seq->tcp.ack = cmd->cl_tcp_ack;
    seq->tcp.psh = cmd->cl_tcp_psh;
    seq->tcp.rst = cmd->cl_tcp_rst;
    seq->tcp.fin = cmd->cl_tcp_fin;
    seq->tcp.urg = cmd->cl_tcp_urg;
    seq->tcp.ece = cmd->cl_tcp_ece;
    seq->tcp.cwr = cmd->cl_tcp_cwr;

    seq->icmp.code = cmd->cl_icmp_code;
    seq->icmp.type = cmd->cl_icmp_type;

    struct payload_opt *pl = &seq->pls[0];

    pl->min_len = cmd->cl_pl_min_len;
    pl->max_len = cmd->cl_pl_max_len;
    pl->is_static = cmd->cl_pl_is_static;

    if (cmd->cl_pl_exact != NULL)
    {
        pl->exact = cmd->cl_pl_exact;
    }

    pl->is_file = cmd->cl_pl_is_file;
    pl->is_string = cmd->cl_pl_is_string;
}

/**
 * Parses the command line options using getopt.
 * 
 * @param argc The argument counter passed in the `int main()` function.
 * @param argv The argument array pointer passed in the `int main()` function.
 * @param cmd A pointer to the `cmd_line` structure that stores all command line values.
 * 
 * @return void
**/
void parse_cmd_line(int argc, char **argv, struct cmd_line *cmd)
{
    int c = -1;

    while ((c = getopt_long(argc, argv, "c:zhvl", long_opts, NULL)) != -1)
    {
        switch (c)
        {
            case 'c':
                cmd->config = optarg;

                break;

            case 'z':
                cmd->cli = 1;

                break;

            /* CLI options. */
            case 0:
                cmd->cl_interface = optarg;

                break;

            case 2:
                cmd->cl_block = atoi(optarg);

                break;

            case 3:
            {
                char *val = strdup(optarg);
                cmd->cl_count = strtoull((const char *)val, (char **)val, 0);

                break;
            }

            case 4:
            {
                char *val = strdup(optarg);
                cmd->cl_time = strtoull((const char *)val, (char **)val, 0);

                break;
            }

            case 5:
            {
                char *val = strdup(optarg);
                cmd->cl_delay = strtoull((const char *)val, (char **)val, 0);

                break;
            }

            case 6:
                cmd->cl_track_count = atoi(optarg);

                break;

            case 7:
            {
                char *val = strdup(optarg);
                cmd->cl_max_data = strtoull((const char *)val, (char **)val, 0);

                break;
            }

            case 8:
                cmd->cl_threads = atoi(optarg);

                break;

            case 9:
                cmd->cl_l4_csum = atoi(optarg);

                break;

            case 10:
                cmd->cl_src_mac = optarg;

                break;

            case 11:
                cmd->cl_dst_mac = optarg;

                break;

            case 12:
                cmd->cl_ttl_min = atoi(optarg);

                break;

            case 13:
                cmd->cl_ttl_max = atoi(optarg);

                break;

            case 14:
                cmd->cl_id_min = atoi(optarg);

                break;

            case 15:
                cmd->cl_id_max = atoi(optarg);

                break;

            case 16:
                cmd->cl_src_ip = optarg;

                break;

            case 17:
                cmd->cl_dst_ip = optarg;

                break;

            case 18:
                cmd->cl_protocol = optarg;

                break;

            case 19:
                cmd->cl_tos = atoi(optarg);

                break;

            case 20:
                cmd->cl_l3_csum = atoi(optarg);

                break;

            case 21:
                cmd->cl_udp_src_port = atoi(optarg);

                break;

            case 22:
                cmd->cl_udp_dst_port = atoi(optarg);

                break;

            case 23:
                cmd->cl_tcp_src_port = atoi(optarg);

                break;

            case 24:
                cmd->cl_tcp_dst_port = atoi(optarg);

                break;

            case 25:
                cmd->cl_tcp_syn = atoi(optarg);

                break;

            case 26:
                cmd->cl_tcp_ack = atoi(optarg);

                break;

            case 27:
                cmd->cl_tcp_psh = atoi(optarg);

                break;

            case 28:
                cmd->cl_tcp_rst = atoi(optarg);

                break;

            case 29:
                cmd->cl_tcp_fin = atoi(optarg);

                break;

            case 30:
                cmd->cl_tcp_urg = atoi(optarg);

                break;

            case 31:
                cmd->cl_tcp_cooked = atoi(optarg);

                break;

            case 32:
                cmd->cl_pl_min_len = atoi(optarg);

                break;

            case 33:
                cmd->cl_pl_max_len = atoi(optarg);

                break;

            case 34:
                cmd->cl_pl_is_static = atoi(optarg);

                break;

            case 35:
                cmd->cl_pl_exact = optarg;

                break;

            case 36:
                cmd->cl_pl_is_file = atoi(optarg);

                break;

            case 37:
                cmd->cl_pl_is_string = atoi(optarg);

                break;
                
            case 38:
                cmd->cl_icmp_code = atoi(optarg);

                break;
                
            case 39:
                cmd->cl_icmp_type = atoi(optarg);

                break;

            case 40:
                cmd->cl_tcp_one_connection = atoi(optarg);

                break;

            case 41:
                cmd->cl_tcp_ece = atoi(optarg);

                break;

            case 42:
                cmd->cl_tcp_cwr = atoi(optarg);

                break;

            case 'l':
                cmd->list = 1;

                break;

            case 'v':
                cmd->verbose = 1;

                break;

            case 'h':
                cmd->help = 1;

                break;
        }
    }
}