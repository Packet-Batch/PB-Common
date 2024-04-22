#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/types.h>

#include <json-c/json.h>

#include "config.h"
#include "utils.h"

/**
 * Parses a config file including the main config options and sequences. It then fills out the config structure passed in the function's parameters.
 * 
 * @param file_name The YAML config file to parse.
 * @param cfg A pointer to a config structure that'll be filled in with values.
 * @param only_seq If set to 1, this function will only parse sequences and add onto the number.
 * @param seq_num A pointer to the current sequence # (starting from 0).
 * 
 * @return Returns 0 on success and -1 on failure.
**/
int parse_config(const char file_name[], struct config *cfg, int only_seq, int *seq_num, __u8 log)
{    
    // Attempt to open config file with JSON.
    json_object *root = json_object_from_file(file_name);

    if (root == NULL)
    {
        fprintf(stderr, "Failed to open config file '%s'.\n", file_name);

        return 1;
    }

    json_object *tmp_obj;

    // Parse global interface.
    if (json_object_object_get_ex(root, "interface", &tmp_obj))
    {
        cfg->interface = (char *) json_object_get_string(tmp_obj);
    }

    // Parse sequences array.
    struct json_object *j_sequences;

    if (!json_object_object_get_ex(root, "sequences", &j_sequences))
    {
        fprintf(stderr, "Failed to open sequences array.\n");

        return 1;
    }

    // Check length of sequences.
    int seq_len = json_object_array_length(j_sequences);

    if (seq_len > 0)
    {
        // Loop through each sequence.
        for (int i = 0; i < seq_len; i++)
        {
            // Retrieve current sequence.
            struct sequence *seq = &cfg->seq[i];

            // Get sequence JSON object.
            json_object *seq_obj = json_object_array_get_idx(j_sequences, i);

            // Retrieve interface.
            if (json_object_object_get_ex(seq_obj, "interface", &tmp_obj))
            {
                seq->interface = (char *) json_object_get_string(tmp_obj);
            }

            // Retrieve block.
            if (json_object_object_get_ex(seq_obj, "block", &tmp_obj))
            {
                seq->block = json_object_get_boolean(tmp_obj);
            }

            // Retrieve max count.
            if (json_object_object_get_ex(seq_obj, "maxcount", &tmp_obj))
            {
                seq->max_count = json_object_get_uint64(tmp_obj);
            }

            // Retrieve max data.
            if (json_object_object_get_ex(seq_obj, "maxdata", &tmp_obj))
            {
                seq->max_data = json_object_get_uint64(tmp_obj);
            }

            // Retrieve time
            if (json_object_object_get_ex(seq_obj, "time", &tmp_obj))
            {
                seq->time = json_object_get_int(tmp_obj);
            }

            // Retrieve threads.
            if (json_object_object_get_ex(seq_obj, "threads", &tmp_obj))
            {
                seq->threads = json_object_get_int(tmp_obj);
            }

            // Retrieve delay.
            if (json_object_object_get_ex(seq_obj, "delay", &tmp_obj))
            {
                seq->delay = json_object_get_uint64(tmp_obj);
            }

            // Retrieve track count.
            if (json_object_object_get_ex(seq_obj, "trackcount", &tmp_obj))
            {
                seq->track_count = json_object_get_boolean(tmp_obj);
            }

            // Retrieve packets.
            json_object *pckts_obj;

            if (!json_object_object_get_ex(seq_obj, "packets", &pckts_obj))
            {
                continue;
            }

            int pckts_len = json_object_array_length(pckts_obj);

            if (pckts_len < 1)
            {
                continue;
            }

            for (int j = 0; j < pckts_len; j++)
            {
                struct packet *pckt = &seq->pckts[j];

                struct json_object *pckt_obj = json_object_array_get_idx(pckts_obj, j);

                // Retrieve layer-4 checksum.
                if (json_object_object_get_ex(pckt_obj, "l4csum", &tmp_obj))
                {
                    pckt->l4_csum = json_object_get_boolean(tmp_obj);
                }

                // Retrieve ethernet object.
                struct json_object *eth_obj;

                if (json_object_object_get_ex(pckt_obj, "eth", &eth_obj))
                {
                    // Source MAC address.
                    if (json_object_object_get_ex(eth_obj, "smac", &tmp_obj))
                    {
                        pckt->eth.src_mac = (char *) json_object_get_string(tmp_obj);
                    }

                    // Destination MAC address.
                    if (json_object_object_get_ex(eth_obj, "dmac", &tmp_obj))
                    {
                        pckt->eth.dst_mac = (char *) json_object_get_string(tmp_obj);
                    }
                }

                // Retrieve IP object.
                struct json_object *ip_obj;

                if (json_object_object_get_ex(pckt_obj, "ip", &ip_obj))
                {
                    // Source IP.
                    if (json_object_object_get_ex(ip_obj, "srcip", &tmp_obj))
                    {
                        pckt->ip.src_ip = (char *) json_object_get_string(tmp_obj);
                    }

                    // Destination IP.
                    if (json_object_object_get_ex(ip_obj, "dstip", &tmp_obj))
                    {
                        pckt->ip.dst_ip = (char *) json_object_get_string(tmp_obj);
                    }

                    // Protocol.
                    if (json_object_object_get_ex(ip_obj, "protocol", &tmp_obj))
                    {
                        pckt->ip.protocol = (char *) json_object_get_string(tmp_obj);
                    }

                    // ToS.
                    if (json_object_object_get_ex(ip_obj, "tos", &tmp_obj))
                    {
                        pckt->ip.tos = json_object_get_int(tmp_obj);
                    }

                    // Checksum.
                    if (json_object_object_get_ex(ip_obj, "csum", &tmp_obj))
                    {
                        pckt->ip.csum = json_object_get_boolean(tmp_obj);
                    }

                    // TTL object.
                    struct json_object *ttl_obj;

                    if (json_object_object_get_ex(ip_obj, "ttl", &ttl_obj))
                    {
                        // Minimum TTL.
                        if (json_object_object_get_ex(ttl_obj, "min", &tmp_obj))
                        {
                            pckt->ip.min_ttl = json_object_get_int(tmp_obj);
                        }

                        // Maximum TTL.
                        if (json_object_object_get_ex(ttl_obj, "max", &tmp_obj))
                        {
                            pckt->ip.max_ttl = json_object_get_int(tmp_obj);
                        }
                    }

                    // ID object.
                    struct json_object *id_obj;

                    if (json_object_object_get_ex(ip_obj, "id", &id_obj))
                    {
                        // Minimum ID.
                        if (json_object_object_get_ex(id_obj, "min", &tmp_obj))
                        {
                            pckt->ip.min_id = json_object_get_int(tmp_obj);
                        }

                        // Maximum ID.
                        if (json_object_object_get_ex(id_obj, "max", &tmp_obj))
                        {
                            pckt->ip.max_id = json_object_get_int(tmp_obj);
                        }
                    }

                    // Ranges array.
                    struct json_object *ranges_obj;

                    if (json_object_object_get_ex(ip_obj, "ranges", &ranges_obj))
                    {
                        int ranges_len = json_object_array_length(ranges_obj);

                        if (ranges_len > 0)
                        {
                            for (int n = 0; n < ranges_len; n++)
                            {
                                // Retrieve specific range and add to ranges array.
                                struct json_object *range_obj = json_object_array_get_idx(ranges_obj, n);

                                pckt->ip.ranges[n] = (char *) json_object_get_string(range_obj);
                                pckt->ip.range_count++;
                            }
                        }
                    }
                }

                // UDP object.
                struct json_object *udp_obj;

                if (json_object_object_get_ex(pckt_obj, "udp", &udp_obj))
                {
                    // Source port.
                    if (json_object_object_get_ex(udp_obj, "srcport", &tmp_obj))
                    {
                        pckt->udp.src_port = json_object_get_int(tmp_obj);
                    }

                    // Destination port.
                    if (json_object_object_get_ex(udp_obj, "dstport", &tmp_obj))
                    {
                        pckt->udp.dst_port = json_object_get_int(tmp_obj);
                    }
                }

                // TCP object.
                struct json_object *tcp_obj;

                if (json_object_object_get_ex(pckt_obj, "tcp", &tcp_obj))
                {
                    // Cooked socket.
                    if (json_object_object_get_ex(tcp_obj, "cooked", &tmp_obj))
                    {
                        pckt->tcp.cooked = json_object_get_boolean(tmp_obj);
                    }

                    // One connection.
                    if (json_object_object_get_ex(tcp_obj, "oneconnection", &tmp_obj))
                    {
                        pckt->tcp.one_connection = json_object_get_boolean(tmp_obj);
                    }

                    // Source port.
                    if (json_object_object_get_ex(tcp_obj, "srcport", &tmp_obj))
                    {
                        pckt->tcp.src_port = json_object_get_int(tmp_obj);
                    }

                    // Destination port.
                    if (json_object_object_get_ex(tcp_obj, "dstport", &tmp_obj))
                    {
                        pckt->tcp.dst_port = json_object_get_int(tmp_obj);
                    }

                    // SYN flag.
                    if (json_object_object_get_ex(tcp_obj, "syn", &tmp_obj))
                    {
                        pckt->tcp.syn = json_object_get_boolean(tmp_obj);
                    }

                    // PSH flag.
                    if (json_object_object_get_ex(tcp_obj, "psh", &tmp_obj))
                    {
                        pckt->tcp.psh = json_object_get_boolean(tmp_obj);
                    }

                    // FIN flag.
                    if (json_object_object_get_ex(tcp_obj, "fin", &tmp_obj))
                    {
                        pckt->tcp.fin = json_object_get_boolean(tmp_obj);
                    }

                    // ACK flag.
                    if (json_object_object_get_ex(tcp_obj, "ack", &tmp_obj))
                    {
                        pckt->tcp.ack = json_object_get_boolean(tmp_obj);
                    }

                    // RST flag.
                    if (json_object_object_get_ex(tcp_obj, "rst", &tmp_obj))
                    {
                        pckt->tcp.rst = json_object_get_boolean(tmp_obj);
                    }

                    // URG flag.
                    if (json_object_object_get_ex(tcp_obj, "urg", &tmp_obj))
                    {
                        pckt->tcp.urg = json_object_get_boolean(tmp_obj);
                    }

                    // ECE flag.
                    if (json_object_object_get_ex(tcp_obj, "ece", &tmp_obj))
                    {
                        pckt->tcp.ece = json_object_get_boolean(tmp_obj);
                    }

                    // CWR flag.
                    if (json_object_object_get_ex(tcp_obj, "cwr", &tmp_obj))
                    {
                        pckt->tcp.cwr = json_object_get_boolean(tmp_obj);
                    }
                }

                // ICMP object.
                struct json_object *icmp_obj;

                if (json_object_object_get_ex(pckt_obj, "icmp", &icmp_obj))
                {
                    // Code.
                    if (json_object_object_get_ex(icmp_obj, "code", &tmp_obj))
                    {
                        pckt->icmp.code = json_object_get_int(tmp_obj);
                    }

                    // Type.
                    if (json_object_object_get_ex(icmp_obj, "type", &tmp_obj))
                    {
                        pckt->icmp.type = json_object_get_int(tmp_obj);
                    }
                }

                // Payload object.
                struct json_object *pl_obj;

                if (json_object_object_get_ex(pckt_obj, "payload", &pl_obj))
                {
                    // Is static.
                    if (json_object_object_get_ex(pl_obj, "isstatic", &tmp_obj))
                    {
                        pckt->pl.is_static = json_object_get_boolean(tmp_obj);
                    }

                    // Is file.
                    if (json_object_object_get_ex(pl_obj, "isfile", &tmp_obj))
                    {
                        pckt->pl.is_file = json_object_get_boolean(tmp_obj);
                    }

                    // Is string.
                    if (json_object_object_get_ex(pl_obj, "isstring", &tmp_obj))
                    {
                        pckt->pl.is_string = json_object_get_boolean(tmp_obj);
                    }

                    // Exact.
                    if (json_object_object_get_ex(pl_obj, "exact", &tmp_obj))
                    {
                        pckt->pl.exact = (char *) json_object_get_string(tmp_obj);
                    }

                    // Length object.
                    struct json_object *len_obj;

                    if (json_object_object_get_ex(pl_obj, "length", &len_obj))
                    {
                        // Minimum length.
                        if (json_object_object_get_ex(len_obj, "min", &tmp_obj))
                        {
                            pckt->pl.min_len = json_object_get_int(tmp_obj);
                        }

                        // Maximum length.
                        if (json_object_object_get_ex(len_obj, "max", &tmp_obj))
                        {
                            pckt->pl.max_len = json_object_get_int(tmp_obj);
                        }
                    }
                }

                seq->pckts_cnt++;

            }

            *seq_num += 1;
        }
    }

    return 0;
}

/**
 * Clears a sequence.
 * 
 * @param cfg A pointer to the config structure.
 * @param seq_num Which sequence to reset.
 * 
 * @return Void
**/
void clear_sequence(struct config *cfg, int seq_num)
{
    cfg->seq[seq_num].interface = NULL;
    cfg->seq[seq_num].block = 1;
    cfg->seq[seq_num].max_count = 0;
    cfg->seq[seq_num].threads = 0;
    cfg->seq[seq_num].time = 0;
    cfg->seq[seq_num].delay = 1000000;

    for (int i = 0; i < MAX_PACKETS; i++)
    {
        struct packet *pkt = &cfg->seq[seq_num].pckts[i];

        pkt->eth.src_mac = NULL;
        pkt->eth.dst_mac = NULL;

        pkt->ip.src_ip = NULL;
        pkt->ip.dst_ip = NULL;
        pkt->ip.protocol = NULL;
        pkt->ip.tos = 0;
        pkt->ip.min_ttl = 64;
        pkt->ip.max_ttl = 64;
        pkt->ip.min_id = 0;
        pkt->ip.max_id = 64000;
        pkt->ip.csum = 1;
        
        pkt->udp.src_port = 0;
        pkt->udp.dst_port = 0;

        pkt->tcp.syn = 0;
        pkt->tcp.ack = 0;
        pkt->tcp.psh = 0;
        pkt->tcp.rst = 0;
        pkt->tcp.fin = 0;
        pkt->tcp.urg = 0;
        pkt->tcp.cooked = 0;
        pkt->tcp.one_connection = 0;
        
        pkt->icmp.code = 0;
        pkt->icmp.type = 0;

        pkt->l4_csum = 1;

        pkt->pl.exact = NULL;
        pkt->pl.is_file = 0;
        pkt->pl.is_string = 0;
        pkt->pl.is_static = 0;
        pkt->pl.min_len = 0;
        pkt->pl.max_len = 0;
    }

    // Reset includes.
    for (int i = 0; i < cfg->seq[seq_num].include_count; i++)
    {
        cfg->seq[seq_num].includes[i] = NULL;
    }

    // Reset source ranges.
    for (int i = 0; i < MAX_PACKETS; i++)
    {
        for (int j = 0; j < cfg->seq[seq_num].pckts[i].ip.range_count; j++)
        {
            cfg->seq[seq_num].pckts[i].ip.ranges[j] = NULL;
        }
    }
}

/**
 * Clears a sequence.
 * 
 * @param cfg A pointer to the config structure.
 * @param seq_cnt How many sequences we have.
 * 
 * @return Void
**/
void print_config(struct config *cfg, int seq_cnt)
{
    fprintf(stdout, "Found %d sequences.\n", seq_cnt);

    fprintf(stdout, "Got interface => %s.\n", cfg->interface ? cfg->interface : "N/A");

    fprintf(stdout, "Sequences:\n\n--------------------------\n");

    for (int i = 0; i < seq_cnt; i++)
    {
        struct sequence *seq = &cfg->seq[i];

        if (!seq)
            continue;

        fprintf(stdout, "Sequence #%d:\n", i);

        // General settings.
        fprintf(stdout, "\tGeneral\n");
        fprintf(stdout, "\t\tIncludes =>\n");

        if (seq->include_count > 0)
        {
            for (int j = 0; j < seq->include_count; j++)
            {
                fprintf(stdout, "\t\t\t- %s\n", seq->includes[j]);
            }
        }


        fprintf(stdout, "\t\tInterface Override => %s\n", seq->interface ? seq->interface : "N/A");
        fprintf(stdout, "\t\tBlock => %s\n", seq->block ? "True" : "False");
        fprintf(stdout, "\t\tCount => %llu\n", seq->max_count);
        fprintf(stdout, "\t\tTime => %llu\n", seq->time);
        fprintf(stdout, "\t\tDelay => %llu\n", seq->delay);
        fprintf(stdout, "\t\tThreads => %u\n", seq->threads);

        // Packets.
        fprintf(stdout, "\t\tPackets\n");

        for (int i = 0; i < seq->pckts_cnt; i++)
        {
            fprintf(stdout, "\t\t\t#%d\n", i + 1);

            struct packet *pckt = &seq->pckts[i];

            // Ethernet settings.
            fprintf(stdout, "\t\t\t\tEthernet\n");
            fprintf(stdout, "\t\t\t\t\tSource MAC => %s\n", pckt->eth.src_mac ? pckt->eth.src_mac : "N/A");
            fprintf(stdout, "\t\t\t\t\tDestination MAC => %s\n", pckt->eth.dst_mac ? pckt->eth.dst_mac : "N/A");

            // IP settings.
            fprintf(stdout, "\t\t\t\tIP\n");
            fprintf(stdout, "\t\t\t\t\tProtocol => %s\n", pckt->ip.protocol ? pckt->ip.protocol : "N/A");

            fprintf(stdout, "\t\t\t\t\tSource IP => %s\n", pckt->ip.src_ip ? pckt->ip.src_ip : "N/A");
            fprintf(stdout, "\t\t\t\t\tDestination IP => %s\n", pckt->ip.dst_ip ? pckt->ip.dst_ip : "N/A");

            if (pckt->ip.range_count > 0)
            {
                fprintf(stdout, "\t\t\t\t\tRanges:\n");

                for (int j = 0; j < pckt->ip.range_count; j++)
                {
                    fprintf(stdout, "\t\t\t\t\t\t- %s\n", pckt->ip.ranges[j]);
                }
            }

            fprintf(stdout, "\t\t\t\t\tType of Service => %d\n", pckt->ip.tos);
            fprintf(stdout, "\t\t\t\t\tMin TTL => %d\n", pckt->ip.min_ttl);
            fprintf(stdout, "\t\t\t\t\tMax TTL => %d\n", pckt->ip.max_ttl);
            fprintf(stdout, "\t\t\t\t\tMin ID => %d\n", pckt->ip.min_id);
            fprintf(stdout, "\t\t\t\t\tMax ID => %d\n", pckt->ip.max_id);
            fprintf(stdout, "\t\t\t\t\tChecksum => %s\n", pckt->ip.csum ? "Yes" : "No");

            // TCP settings.
            fprintf(stdout, "\t\t\t\tTCP\n");
            fprintf(stdout, "\t\t\t\t\tSource Port => %d\n", pckt->tcp.src_port);
            fprintf(stdout, "\t\t\t\t\tDest Port => %d\n", pckt->tcp.dst_port);
            fprintf(stdout, "\t\t\t\t\tUse Socket => %s\n", pckt->tcp.cooked ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tOne Connection => %s\n", pckt->tcp.one_connection ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tSYN Flag => %s\n", pckt->tcp.syn ? "Yes": "No");
            fprintf(stdout, "\t\t\t\t\tPSH Flag => %s\n", pckt->tcp.psh ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tFIN Flag => %s\n", pckt->tcp.fin ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tACK Flag => %s\n", pckt->tcp.ack ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tRST Flag => %s\n", pckt->tcp.rst ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tURG Flag => %s\n", pckt->tcp.urg ? "Yes" : "No");

            // UDP settings.
            fprintf(stdout, "\t\t\t\tUDP\n");
            fprintf(stdout, "\t\t\t\t\tSrc Port => %d\n", pckt->udp.src_port);
            fprintf(stdout, "\t\t\t\t\tDst Port => %d\n", pckt->udp.dst_port);

            // ICMP settings.
            fprintf(stdout, "\t\t\t\tICMP\n");
            fprintf(stdout, "\t\t\t\t\tCode => %d\n", pckt->icmp.code);
            fprintf(stdout, "\t\t\t\t\tType => %d\n", pckt->icmp.type);

            // Layer 4 setting(s).
            fprintf(stdout, "\t\t\t\tLayer 4\n");
            fprintf(stdout, "\t\t\t\t\tChecksum => %s\n", pckt->l4_csum ? "Yes" : "No");

            // Payload settings.
            fprintf(stdout, "\t\t\t\tPayload\n");
            fprintf(stdout, "\t\t\t\t\tMin Length => %d\n", pckt->pl.min_len);
            fprintf(stdout, "\t\t\t\t\tMax Length => %d\n", pckt->pl.max_len);
            fprintf(stdout, "\t\t\t\t\tIs Static => %s\n", pckt->pl.is_static ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tIs File => %s\n", pckt->pl.is_file ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tIs String => %s\n", pckt->pl.is_string ? "Yes" : "No");
            fprintf(stdout, "\t\t\t\t\tExact String => %s\n", pckt->pl.exact ? pckt->pl.exact : "N/A");

            fprintf(stdout, "\n\n");
        }
    }
}