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

    {"srcmac", required_argument, NULL, 10},
    {"dstmac", required_argument, NULL, 11},

    {"minttl", required_argument, NULL, 12},
    {"maxttl", required_argument, NULL, 13},
    {"minid", required_argument, NULL, 14},
    {"maxid", required_argument, NULL, 15},
    {"srcip", required_argument, NULL, 16},
    {"dstip", required_argument, NULL, 17},
    {"protocol", required_argument, NULL, 18},
    {"tos", required_argument, NULL, 19},
    {"l3csum", required_argument, NULL, 20},

    {"usrcport", required_argument, NULL, 21},
    {"udstport", required_argument, NULL, 22},

    {"tsrcport", required_argument, NULL, 23},
    {"tdstport", required_argument, NULL, 24},
    {"tsyn", required_argument, NULL, 25},
    {"tack", required_argument, NULL, 26},
    {"tpsh", required_argument, NULL, 27},
    {"trst", required_argument, NULL, 28},
    {"tfin", required_argument, NULL, 29},
    {"turg", required_argument, NULL, 30},
    {"tusesocket", required_argument, NULL, 31},
    {"oneconnection", required_argument, NULL, 40},

    {"icode", required_argument, NULL, 38},
    {"itype", required_argument, NULL, 39},

    {"pmin", required_argument, NULL, 32},
    {"pmax", required_argument, NULL, 33},
    {"pstatic", required_argument, NULL, 34},
    {"pexact", required_argument, NULL, 35},
    {"pfile", required_argument, NULL, 36},
    {"pstring", required_argument, NULL, 37},

    {"list", no_argument, NULL, 'l'},
    {"verbose", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

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
    /* Parse main options. */
    if (cmd->cl_interface != NULL)
    {
        cfg->interface = cmd->cl_interface;
    }

    cfg->seq[0].block = cmd->cl_block;
    cfg->seq[0].count = cmd->cl_count;
    cfg->seq[0].time = cmd->cl_time;
    cfg->seq[0].delay = cmd->cl_delay;
    cfg->seq[0].track_count = cmd->cl_track_count;
    cfg->seq[0].max_data = cmd->cl_max_data;
    cfg->seq[0].threads = cmd->cl_threads;
    cfg->seq[0].l4_csum = cmd->cl_l4_csum;

    cfg->seq[0].eth.src_mac = cmd->cl_src_mac;
    cfg->seq[0].eth.dst_mac = cmd->cl_dst_mac;

    cfg->seq[0].ip.min_ttl = cmd->cl_ttl_min;
    cfg->seq[0].ip.max_ttl = cmd->cl_ttl_max;
    cfg->seq[0].ip.min_id = cmd->cl_id_min;
    cfg->seq[0].ip.max_id = cmd->cl_id_max;

    if (cmd->cl_src_ip != NULL)
    {
        // Check for range.
        if (strstr(cmd->cl_src_ip, "/") != NULL)
        {
            cfg->seq[0].ip.src_ip = 0;
            cfg->seq[0].ip.range_count = 1;
            cfg->seq[0].ip.ranges[0] = cmd->cl_src_ip;
        }
        else
        {
            cfg->seq[0].ip.src_ip = cmd->cl_src_ip;
        }
    }

    if (cmd->cl_dst_ip != NULL)
    {
        cfg->seq[0].ip.dst_ip = cmd->cl_dst_ip;
    }

    cfg->seq[0].ip.protocol = cmd->cl_protocol;
    cfg->seq[0].ip.tos = cmd->cl_tos;
    cfg->seq[0].ip.csum = cmd->cl_l3_csum;

    cfg->seq[0].udp.src_port = cmd->cl_udp_src_port;
    cfg->seq[0].udp.dst_port = cmd->cl_udp_dst_port;

    cfg->seq[0].tcp.src_port = cmd->cl_tcp_src_port;
    cfg->seq[0].tcp.dst_port = cmd->cl_tcp_dst_port;
    cfg->seq[0].tcp.syn = cmd->cl_tcp_syn;
    cfg->seq[0].tcp.ack = cmd->cl_tcp_ack;
    cfg->seq[0].tcp.psh = cmd->cl_tcp_psh;
    cfg->seq[0].tcp.rst = cmd->cl_tcp_rst;
    cfg->seq[0].tcp.fin = cmd->cl_tcp_fin;
    cfg->seq[0].tcp.urg = cmd->cl_tcp_urg;
    cfg->seq[0].tcp.use_socket = cmd->cl_tcp_use_socket;
    cfg->seq[0].tcp.one_connection = cmd->cl_tcp_one_connection;

    cfg->seq[0].icmp.code = cmd->cl_icmp_code;
    cfg->seq[0].icmp.type = cmd->cl_icmp_type;

    cfg->seq[0].pl.min_len = cmd->cl_pl_min_len;
    cfg->seq[0].pl.max_len = cmd->cl_pl_max_len;
    cfg->seq[0].pl.is_static = cmd->cl_pl_is_static;

    if (cmd->cl_pl_exact != NULL)
    {
        cfg->seq[0].pl.exact = cmd->cl_pl_exact;
    }

    cfg->seq[0].pl.is_file = cmd->cl_pl_is_file;
    cfg->seq[0].pl.is_string = cmd->cl_pl_is_string;
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
                cmd->cl_tcp_use_socket = atoi(optarg);

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