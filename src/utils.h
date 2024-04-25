#pragma once

#include <linux/types.h>

void get_gw_mac(__u8 *mac);
int get_src_mac_address(const char *dev, __u8 *src_mac);
__u16 rand_num(__u16 min, __u16 max, unsigned int seed);
char *lower_str(char *str);
char *rand_ip(char *range, unsigned int seed);