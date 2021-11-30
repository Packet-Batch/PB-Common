#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <linux/types.h>

#include <arpa/inet.h>

/**
 * Retrieves the Ethernet MAC of the host's default gateway and stores it in `mac` (__u8 *).
 * 
 * @param mac The variable to store the MAC address in. Must be an __u8 * array with the length of ETH_ALEN (6).
 * 
 * @return Void
**/
void get_gw_mac(__u8 *mac)
{
    char cmd[] = "ip neigh | grep \"$(ip -4 route list 0/0|cut -d' ' -f3) \"|cut -d' ' -f5|tr '[a-f]' '[A-F]'";

    FILE *fp =  popen(cmd, "r");

    if (fp != NULL)
    {
        char line[18];

        if (fgets(line, sizeof(line), fp) != NULL)
        {
            sscanf(line, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        }

        pclose(fp);
    }
}

/**
 * Returns a random integer between min and max using rand_r(), a thread-safe function. 
 * 
 * @param min The minimum number to choose from.
 * @param max The maximum number to choose from.
 * @param seed The seed to pass to the rand_r() function.
 * 
 * @return A 16-bit integer (__u16).
 * 
 * @note If you're trying to return an integer within the 8-bit range, I'd recommend casting as __u8 or similar.
**/
__u16 rand_num(__u16 min, __u16 max, unsigned int seed)
{
    return (rand_r(&seed) % (max - min + 1)) + min;
}

/**
 * Simply lower-cases a string.
 * 
 * @param str Pointer to the full string we want to lower-case.
 * 
 * @return A character pointer to the lower-cased string.
**/
char *lower_str(char *str) 
{
    for (char *p = str; *p; p++) 
    {
        *p = tolower(*p);
    }

    return str;
}

/**
 * Chooses a random IP from a specific CIDR range.
 * 
 * @param range The range in IP/CIDR format.
 * 
 * @return The pointer to a string with the random IP within the CIDR range.
 * 
 * @note Thanks for the help on https://stackoverflow.com/questions/64542446/choosing-a-random-ip-from-any-specific-cidr-range-in-c.
**/
char *rand_ip(char *range, __u16 *pckt_count)
{
    // Split the <ip>/<cidr> and assign both values.
    char *split;

    char *s_ip = NULL;    
    char *cidr_str = NULL;

    char *str = malloc(sizeof(char) * (strlen(range) + 1));
    strcpy(str, range);

    split = strtok(str, "/");

    // Check to ensure split isn't NULL.
    if (split != NULL)
    {
        // Get network IP.
        s_ip = strdup(split);

        // Split again based off of '/'.
        split = strtok(NULL, "/");

        // Check to ensure split isn't NULL and get CIDR.
        if (split != NULL)
        {
            cidr_str = strdup(split);
        }
    }

    // Check to ensure CIDR string and source IP isn't NULL.
    if (cidr_str == NULL || s_ip == NULL)
    {
        return "127.0.0.1";
    }

    // Free the temporary string (str).
    free(str);

    __u8 cidr = (__u8) atoi(cidr_str);

    // Randomize the rand_r() seed.
    unsigned int seed = time(NULL) + (unsigned long)*pckt_count;

    // Create in_addr and convert the IP string to a 32-bit integer.
    struct in_addr in_addr;
    inet_aton(s_ip, &in_addr);
    __u32 ip_addr = ntohl(in_addr.s_addr);

    // Get the mask (the complement of 2 to the power of the CIDR minus one).
    __u32 mask = (1 << (32 - cidr)) - 1;

    // Generate a random number using rand_r(&seed).
    __u32 rand_num = rand_r(&seed);

    // Generate new 32-bit IPv4 address from IP/CIDR range above.
    __u32 rand_ip = (ip_addr & ~mask) | (mask & rand_num);

    // Convert the new IP to a string and print it.
    struct in_addr rand_ip_str;
    rand_ip_str.s_addr = htonl(rand_ip);

    return inet_ntoa(rand_ip_str);
}