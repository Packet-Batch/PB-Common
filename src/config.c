#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/types.h>

#include <yaml.h>

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
    // Attempt to open config file.
    FILE *fp = fopen(file_name, "r");

    // Check if file pointer is valid.
    if (fp == NULL)
    {
        if (log)
        {
            fprintf(stderr, "Error opening YAML config file (%s) :: %s.\n", file_name, strerror(errno));
        }

        return -1;
    }

    // Create YAML variables.
    yaml_parser_t parser;
    yaml_event_t ev;

    // Initialize parser.
    if (!yaml_parser_initialize(&parser))
    {
        if (log)
        {
            fprintf(stderr, "Error initializing YAML parser (#%d) :: %s.\n", parser.error, strerror(errno));
        }

        return -1;
    }

    // Set parser's input file.
    yaml_parser_set_input_file(&parser, fp);

    // General YAML.
    char *prev_key = NULL;
    //char *prevsec;

    // Sequences.
    int in_sequence = 0;
    int in_sequences = 0;
    char *cur_seq;

    // Sequence-specific.
    int in_includes = 0;
    int in_eth = 0;
    int in_ip = 0;
    int in_udp = 0;
    int in_tcp = 0;
    int in_icmp = 0;
    int in_payload = 0;

    // Additional IP.
    int in_ttl = 0;
    int in_ranges = 0;
    int in_id = 0;
    
    // Additional pl.
    int in_length = 0;
    
    do
    {
        // Keep scanning.
        if (!yaml_parser_parse(&parser, &ev))
        {
            if (log)
            {
                fprintf(stderr, "Error parsing YAML file (#%d) :: %s.\n", parser.error, strerror(errno));
            }

            return -1;
        }

        switch (ev.type)
        {
            case YAML_MAPPING_START_EVENT:
                // This occurs when we start a new mapping section. We want to check the previous key to see if it matches a mapping section we're expecting.
                
                // Check if we're already inside a sequence.
                if (in_sequence)
                {
                    // Check if we're in an existing mapping within the sequence.
                    if (in_ip)
                    {
                        // Now check if we're entering the TTL mapping.
                        if (prev_key != NULL && !strcmp(prev_key, "ttl"))
                        {
                            in_ttl = 1;
                        }
                        // Check if we're entering the ID mapping.
                        else if (prev_key != NULL && !strcmp(prev_key, "id"))
                        {
                            in_id = 1;
                        }
                    }

                    if (in_payload)
                    {
                        if (prev_key != NULL && !strcmp(prev_key, "length"))
                        {
                            in_length = 1;
                        }
                    }

                    // Check for additional mappings inside a single sequence.
                    if (!in_eth && prev_key != NULL && !strcmp(prev_key, "eth"))
                    {
                        in_eth = 1;
                    }

                    if (!in_ip && prev_key != NULL && !strcmp(prev_key, "ip"))
                    {
                        in_ip = 1;
                    }

                    if (!in_udp && prev_key != NULL && !strcmp(prev_key, "udp"))
                    {
                        in_udp = 1;
                    }

                    if (!in_tcp && prev_key != NULL && !strcmp(prev_key, "tcp"))
                    {
                        in_tcp = 1;
                    }

                    if (!in_icmp && prev_key != NULL && !strcmp(prev_key, "icmp"))
                    {
                        in_icmp = 1;
                    }

                    if (!in_payload && prev_key != NULL && !strcmp(prev_key, "payload"))
                    {
                        in_payload = 1;
                    }
                }
                

                // Check for start of sequences.
                if (in_sequences == 0 && prev_key != NULL && !strcmp(prev_key, "sequences"))
                {
                    // We're now inside of the sequences map.
                    in_sequences = 1;
                }
                
                // Check if we're inside sequences already, but not inside of a single sequence.
                if (in_sequences && !in_sequence)
                {
                    // We're now entering a separate sequence.
                    in_sequence = 1;

                    cur_seq = strdup(prev_key);
                }

                break;
            
            case YAML_MAPPING_END_EVENT:
                // Check if we're in inside of sequences.
                if (in_sequences)
                {
                    // Check if we're inside a single sequence.
                    if (in_sequence)
                    {
                        // Now go through each mapping inside of a single sequence and do additional checks.
                        if (in_includes)
                        {
                            in_includes = 0;
                        }
                        else if (in_eth)
                        {
                            in_eth = 0;
                        }
                        else if (in_ip)
                        {
                            // Check for TTL mapping.
                            if (in_ttl)
                            {
                                in_ttl = 0;
                            }
                            else if (in_id)
                            {
                                in_id = 0;
                            }
                            else
                            {
                                in_ip = 0;
                            }
                        }
                        else if (in_udp)
                        {
                            in_udp = 0;
                        }
                        else if (in_tcp)
                        {
                            in_tcp = 0;
                        }
                        else if (in_icmp)
                        {
                            in_icmp = 0;
                        }
                        else if (in_payload)
                        {
                            // Check if we're in length mapping.
                            if (in_length)
                            {
                                in_length = 0;
                            }
                            else
                            {
                                in_payload = 0;
                            }
                        }
                        else
                        {
                            // Since everything else wasn't set, we should be exiting the sequence.
                            in_sequence = 0;

                            // Increase sequence count since the last one should have ended.
                            (*seq_num)++;
                        }
                    }
                    else
                    {
                        // We should be exiting sequences all together in this case.
                        in_sequences = 0;
                    }
                }
                
                break;

            case YAML_SEQUENCE_START_EVENT:
                // Check for includes or ranges.
                if (in_sequence)
                {
                    if (!in_includes && prev_key != NULL && !strcmp(prev_key, "includes"))
                    {
                        in_includes = 1;
                    }

                    if (!in_ranges && in_ip && prev_key != NULL && !strcmp(prev_key, "ranges"))
                    {
                        in_ranges = 1;
                    }
                }

                break;

            case YAML_SEQUENCE_END_EVENT:
                // Check if we're exiting includes or ranges.
                if (in_sequence && in_includes)
                {
                    in_includes = 0;
                }

                if (in_sequence && in_ip && in_ranges)
                {
                    in_ranges = 0;
                }
            
                break;

            case YAML_SCALAR_EVENT:
                // We want to check keys and values within the scalar (typically `key: value`).

                if (parser.state == YAML_PARSE_BLOCK_MAPPING_VALUE_STATE)
                {
                    // Assign prev_key to the value since this is representing a key.
                    prev_key = strdup((const char *)ev.data.scalar.value);
                }
                else if (parser.state == YAML_PARSE_BLOCK_MAPPING_KEY_STATE || parser.state == YAML_PARSE_BLOCK_SEQUENCE_ENTRY_STATE)
                {
                    // Check if we're within a sequence or not.
                    if (in_sequence)
                    {
                        // Check if we're within mappings inside the sequence.
                        if (in_includes)
                        {
                            // Since we don't care about the key, just add onto the structure and increment the count.
                            cfg->seq[*seq_num].includes[cfg->seq[*seq_num].include_count] = strdup((const char *)ev.data.scalar.value);

                            // We're going to parse the include here. The 'includes' MUST be at the beginning of the sequence. Otherwise, it will overwrite the current sequence values.
                            parse_config(cfg->seq[*seq_num].includes[cfg->seq[*seq_num].include_count], cfg, 1, seq_num, 1);

                            // Increment count.
                            cfg->seq[*seq_num].include_count++;
                        }
                        else if (in_eth)
                        {
                            // Check for source MAC.
                            if (prev_key != NULL && !strcmp(prev_key, "smac"))
                            {
                                cfg->seq[*seq_num].eth.src_mac = strdup((const char *)ev.data.scalar.value);
                            }

                            // Check for destination MAC.
                            if (prev_key != NULL && !strcmp(prev_key, "dmac"))
                            {
                                cfg->seq[*seq_num].eth.dst_mac = strdup((const char *)ev.data.scalar.value);
                            }
                        }
                        else if (in_ip)
                        {
                            // Check if we're within the TTL mapping.
                            if (in_ttl)
                            {
                                // Check for min TTL.
                                if (prev_key != NULL && !strcmp(prev_key, "min"))
                                {
                                    cfg->seq[*seq_num].ip.min_ttl = (__u8) atoi((const char *)ev.data.scalar.value);
                                }

                                // Check for max TTL.
                                if (prev_key != NULL && !strcmp(prev_key, "max"))
                                {
                                    cfg->seq[*seq_num].ip.max_ttl = (__u8) atoi((const char *)ev.data.scalar.value);
                                }
                            }
                            else if (in_id)
                            {
                                // Check for min ID.
                                if (prev_key != NULL && !strcmp(prev_key, "min"))
                                {
                                    cfg->seq[*seq_num].ip.min_id = (__u16) atoi((const char *)ev.data.scalar.value);
                                }

                                // Check for max ID.
                                if (prev_key != NULL && !strcmp(prev_key, "max"))
                                {
                                    cfg->seq[*seq_num].ip.max_id = (__u16) atoi((const char *)ev.data.scalar.value);
                                }  
                            }
                            else if (in_ranges)
                            {
                                // Since we don't care about the key in ranges, simply add it and increase range count.
                                cfg->seq[*seq_num].ip.ranges[cfg->seq[*seq_num].ip.range_count] = strdup((const char *)ev.data.scalar.value);

                                cfg->seq[*seq_num].ip.range_count++;
                            }
                            else
                            {
                                // Look for all other IP options.

                                // Check for source IP.
                                if (prev_key != NULL && !strcmp(prev_key, "srcip"))
                                {
                                    cfg->seq[*seq_num].ip.src_ip = strdup((const char *)ev.data.scalar.value);
                                }

                                // Check for destination IP.
                                if (prev_key != NULL && !strcmp(prev_key, "dstip"))
                                {
                                    cfg->seq[*seq_num].ip.dst_ip = strdup((const char *)ev.data.scalar.value);
                                }

                                // Check for protocol.
                                if (prev_key != NULL && !strcmp(prev_key, "protocol"))
                                {
                                    cfg->seq[*seq_num].ip.protocol = strdup((const char *)ev.data.scalar.value);
                                }

                                // Check for TOS.
                                if (prev_key != NULL && !strcmp(prev_key, "tos"))
                                {
                                    cfg->seq[*seq_num].ip.tos = (__u8) atoi((const char *)ev.data.scalar.value);
                                }

                                // Check for IP checksum calculation.
                                if (prev_key != NULL && !strcmp(prev_key, "csum"))
                                {
                                    cfg->seq[*seq_num].ip.csum = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                                }
                            }
                        }
                        else if (in_udp)
                        {
                            // Check for source port.
                            if (prev_key != NULL && !strcmp(prev_key, "srcport"))
                            {
                                cfg->seq[*seq_num].udp.src_port = (__u16) atoi((const char *)ev.data.scalar.value);
                            }

                            // Check for destination port.
                            if (prev_key != NULL && !strcmp(prev_key, "dstport"))
                            {
                                cfg->seq[*seq_num].udp.dst_port = (__u16) atoi((const char *)ev.data.scalar.value);
                            }
                        }
                        else if (in_tcp)
                        {
                            // Check for source port.
                            if (prev_key != NULL && !strcmp(prev_key, "srcport"))
                            {
                                cfg->seq[*seq_num].tcp.src_port = (__u16) atoi((const char *)ev.data.scalar.value);
                            }

                            // Check for destination port.
                            if (prev_key != NULL && !strcmp(prev_key, "dstport"))
                            {
                                cfg->seq[*seq_num].tcp.dst_port = (__u16) atoi((const char *)ev.data.scalar.value);
                            }

                            // Check for SYN flag.
                            if (prev_key != NULL && !strcmp(prev_key, "syn"))
                            {
                                cfg->seq[*seq_num].tcp.syn = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for ACK flag.
                            if (prev_key != NULL && !strcmp(prev_key, "ack"))
                            {
                                cfg->seq[*seq_num].tcp.ack = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for PSH flag.
                            if (prev_key != NULL && !strcmp(prev_key, "psh"))
                            {
                                cfg->seq[*seq_num].tcp.psh = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for RST flag.
                            if (prev_key != NULL && !strcmp(prev_key, "rst"))
                            {
                                cfg->seq[*seq_num].tcp.rst = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for FIN flag.
                            if (prev_key != NULL && !strcmp(prev_key, "fin"))
                            {
                                cfg->seq[*seq_num].tcp.fin = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for URG flag.
                            if (prev_key != NULL && !strcmp(prev_key, "urg"))
                            {
                                cfg->seq[*seq_num].tcp.urg = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // TCP cooked Linux socket.
                            if (prev_key != NULL && !strcmp(prev_key, "usesocket"))
                            {
                                cfg->seq[*seq_num].tcp.use_socket = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }
                        }
                        else if (in_icmp)
                        {
                            // Check for code.
                            if (prev_key != NULL && !strcmp(prev_key, "code"))
                            {
                                cfg->seq[*seq_num].icmp.code = (__u8) atoi((const char *)ev.data.scalar.value);
                            }

                            // Check for type.
                            if (prev_key != NULL && !strcmp(prev_key, "type"))
                            {
                                cfg->seq[*seq_num].icmp.type = (__u8) atoi((const char *)ev.data.scalar.value);
                            }
                        }
                        else if (in_payload)
                        {
                            // Check if we're inside the length mapping already.
                            if (in_length)
                            {
                                // Check for min length.
                                if (prev_key != NULL && !strcmp(prev_key, "min"))
                                {
                                    cfg->seq[*seq_num].pl.min_len = (__u16) atoi((const char *)ev.data.scalar.value);
                                }

                                // Check for max length.
                                if (prev_key != NULL && !strcmp(prev_key, "max"))
                                {
                                    cfg->seq[*seq_num].pl.max_len = (__u16) atoi((const char *)ev.data.scalar.value);
                                }

                                // Check for static pl.
                                if (prev_key != NULL && !strcmp(prev_key, "static"))
                                {
                                    cfg->seq[*seq_num].pl.is_static = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                                }
                            }
                            else
                            {
                                // Check for exact pl.
                                if (prev_key != NULL && !strcmp(prev_key, "exact"))
                                {
                                    cfg->seq[*seq_num].pl.exact = strdup((const char *)ev.data.scalar.value);
                                }

                                // Check if payload is file.
                                if (prev_key != NULL && !strcmp(prev_key, "isfile"))
                                {
                                    cfg->seq[*seq_num].pl.is_file = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                                }

                                // Check if payload is string.
                                if (prev_key != NULL && !strcmp(prev_key, "isstring"))
                                {
                                    cfg->seq[*seq_num].pl.is_string = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                                }
                            }
                        }
                        else
                        {
                            // Check for other sequence key => values.

                            // Check for interface override.
                            if (prev_key != NULL && !strcmp(prev_key, "interface"))
                            {
                                cfg->seq[*seq_num].interface = strdup((const char *)ev.data.scalar.value);
                            }

                            // Check for block.
                            if (prev_key != NULL && !strcmp(prev_key, "block"))
                            {
                                cfg->seq[*seq_num].block = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for count.
                            if (prev_key != NULL && !strcmp(prev_key, "count"))
                            {
                                cfg->seq[*seq_num].count = strtoull((const char *)ev.data.scalar.value, (char **)ev.data.scalar.value, 0);
                            }

                            // Check for time.
                            if (prev_key != NULL && !strcmp(prev_key, "time"))
                            {
                                cfg->seq[*seq_num].time = strtoull((const char *)ev.data.scalar.value, (char **)ev.data.scalar.value, 0);
                            }

                            // Check for time.
                            if (prev_key != NULL && !strcmp(prev_key, "delay"))
                            {
                                cfg->seq[*seq_num].delay = strtoull((const char *)ev.data.scalar.value, (char **)ev.data.scalar.value, 0);
                            }

                            // Check for max data.
                            if (prev_key != NULL && !strcmp(prev_key, "data"))
                            {
                                cfg->seq[*seq_num].max_data = strtoull((const char *)ev.data.scalar.value, (char **)ev.data.scalar.value, 0);
                            }

                            // Check for tracking count.
                            if (prev_key != NULL && !strcmp(prev_key, "trackcount"))
                            {
                                cfg->seq[*seq_num].track_count = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }

                            // Check for threads.
                            if (prev_key != NULL && !strcmp(prev_key, "threads"))
                            {
                                cfg->seq[*seq_num].threads = atoi((const char *)ev.data.scalar.value);
                            }

                            // Check for layer 4 checksum.
                            if (prev_key != NULL && !strcmp(prev_key, "l4csum"))
                            {   
                                cfg->seq[*seq_num].l4_csum = (!strcmp(lower_str((char *)ev.data.scalar.value), "true")) ? 1 : 0;
                            }
                        }
                    }
                    else
                    {
                        // If we're only parsing sequences, break.
                        if (only_seq)
                        {
                            continue;
                        }

                        // We should be in the global scope. Check for things like the interface.
                        if (prev_key != NULL && !strcmp(prev_key, "interface"))
                        {
                            cfg->interface = strdup((const char *)ev.data.scalar.value);
                        }
                    }
                }

                break;
            
            default:
                break;
        }

        // Check for end of file.
        if (ev.type != YAML_STREAM_END_EVENT)
        {
            yaml_event_delete(&ev);
        }
    } while (ev.type != YAML_STREAM_END_EVENT);

    // Delete token if it isn't already.
    yaml_event_delete(&ev);    

    // Close the YAML parser.
    yaml_parser_delete(&parser);

    // Close config file.
    fclose(fp);

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
    cfg->seq[seq_num].count = 0;
    cfg->seq[seq_num].threads = 0;
    cfg->seq[seq_num].time = 0;
    cfg->seq[seq_num].delay = 1000000;

    cfg->seq[seq_num].eth.src_mac = NULL;
    cfg->seq[seq_num].eth.dst_mac = NULL;

    cfg->seq[seq_num].ip.src_ip = NULL;
    cfg->seq[seq_num].ip.dst_ip = NULL;
    cfg->seq[seq_num].ip.protocol = NULL;
    cfg->seq[seq_num].ip.tos = 0;
    cfg->seq[seq_num].ip.min_ttl = 64;
    cfg->seq[seq_num].ip.max_ttl = 64;
    cfg->seq[seq_num].ip.min_id = 0;
    cfg->seq[seq_num].ip.max_id = 64000;
    cfg->seq[seq_num].ip.csum = 1;

    
    cfg->seq[seq_num].udp.src_port = 0;
    cfg->seq[seq_num].udp.dst_port = 0;

    cfg->seq[seq_num].tcp.syn = 0;
    cfg->seq[seq_num].tcp.ack = 0;
    cfg->seq[seq_num].tcp.psh = 0;
    cfg->seq[seq_num].tcp.rst = 0;
    cfg->seq[seq_num].tcp.fin = 0;
    cfg->seq[seq_num].tcp.urg = 0;
    cfg->seq[seq_num].tcp.use_socket = 0;
    
    cfg->seq[seq_num].icmp.code = 0;
    cfg->seq[seq_num].icmp.type = 0;

    cfg->seq[seq_num].l4_csum = 1;

    cfg->seq[seq_num].pl.exact = NULL;
    cfg->seq[seq_num].pl.is_file = 0;
    cfg->seq[seq_num].pl.is_string = 0;
    cfg->seq[seq_num].pl.is_static = 0;
    cfg->seq[seq_num].pl.min_len = 0;
    cfg->seq[seq_num].pl.max_len = 0;

    // Reset includes.
    for (int i = 0; i < cfg->seq[seq_num].include_count; i++)
    {
        cfg->seq[seq_num].includes[cfg->seq[seq_num].include_count] = NULL;
    }

    // Reset source ranges.
    for (int i = 0; i < cfg->seq[seq_num].ip.range_count; i++)
    {
        cfg->seq[seq_num].ip.ranges[cfg->seq[seq_num].ip.range_count] = NULL;
    }
}