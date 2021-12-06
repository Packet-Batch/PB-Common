# Packet Batch (Common)
[![Packet Batch Common Build Workflow](https://github.com/Packet-Batch/PB-Common/actions/workflows/build.yml/badge.svg)](https://github.com/Packet-Batch/PB-Common/actions/workflows/build.yml)

## Description
This is a repository for Packet Batch that includes common files for all versions of Packet Batch to use (standard, AF_XDP, and the DPDK). This includes source files to parse YAML config files, the command line, and a few other useful functions (utils).

All configuration for Packet Batch will be available in this repository along with the tests/examples repository [here](https://github.com/Packet-Batch/PB-Tests).

Packet Batch is a tool that should only be used for pen-testing and network monitoring. I do **NOT** support using this tool for anything malicious or for targeted attacks. Please use this tool at your **own risk**.

With that said, this tool acts as a DoS (Denial-of-Service) attack tool/application. If launched from multiple sources to the same network/IP, it is considered a **D**DoS (Distributed Denial-of-Service) attack. You may customize a lot of the packet's contents (layer 2/3/4 headers and payload) and launch different types of attacks at once or in a chain via *sequences*. The following are also supported.

* Randomized source IPs via CIDR ranges.
* Randomized payload length (within a minimum and maximum range).
* UDP, TCP, and ICMP layer 4 protocols supported.
* Optional layer 3 and 4 checksum calculation in the event you want the NIC's hardware to calculate checksums for the outgoing packets.

**NOTE** - This project was inspired by my previous Packet Sequence [project](https://github.com/gamemann/Packet-Sequence). Packet Sequence only supported `AF_PACKETv3` Linux sockets, however. In an effort to simplify code, I decided to make a new organization and project which'll support special versions for AF_XDP Linux sockets and the DPDK which should be much faster than the standard version (with the exception of no TCP cooked sockets support).

## Packet Batch Applications/Versions
There are three versions of Packet Batch that will be supported. As of right now, only the standard is completed.

* [Standard](https://github.com/Packet-Batch/PB-Standard) - Uses `AF_PACKETv3` sockets and supports TCP cookied sockets for easy TCP connection establishing.
* [AF_XDP](https://github.com/Packet-Batch/PB-AF-XDP) - Uses `AF_XDP` sockets which is faster than `AF_PACKETv3`, but doesn't support TCP cooked sockets.
* [DPDK](https://github.com/Packet-Batch/PB-DPDK) - Uses [the DPDK](https://dpdk.org) which is faster than all other versions, but since the DPDK is a kernel-bypass library, it is harder to setup and only supports certain hardware. Also doesn't support TCP cooked sockets.

**NOTE** - **NF** means **N**ot **F**inished.

## Dependencies
* [LibYAML](https://github.com/yaml/libyaml) - Used for parsing config files using the YAML syntax.

## YAML Configuration
If you want to use Packet Batch for more than one sequence, you will need to specify sequences inside of a config file using the YAML syntax. Please see the following for an explanation.

**NOTE** - The default config path is `/etc/pcktbatch/pcktbatch.yaml`. This may be changed via the `-c` and `--cfg` flags as explained under the Command Line Usage section below.

```yaml
# The interface to use when sending packets.
interface: NULL

sequences:
    seq01:
        # An array of other configs to include before this sequence. WARNING - If this is used, you must write this at the beginning of the sequence like this example. Otherwise, unexpected results will occur (e.g. the current sequence will be overwritten). This is empty by default and only showing as an example.
        includes:
            - /etc/pcktbatch/include_one.yaml
            - /etc/pcktbatch/include_two.yaml

        # If set, will use a specific interface for this sequence. Otherwise, uses the default interface specified at the beginning of the config.
        interface: NULL

        # If true, future sequences will wait until this one finishes before executing.
        block: true

        # The maximum packets this sequence can produce before terminating.
        count: 0

        # The maximum bytes this sequence can produce before terminating.
        data: 0

        # How long in seconds this sequence can go on before terminating.
        time: 0

        # The amount of threads to spawn with this sequence. If this is set to 0, it will use the CPU count (recommended).
        threads: 0

        # The delay between sending packets on each thread in microseconds.
        delay: 1000000

        # If true, even if 'count' is set to 0, the program will keep a packet counter inside of each thread. As of right now, a timestamp (in seconds) and a packet counter is used to generate a seed for randomness within the packet. If you want true randomness with every packet and not with each second, it is recommended you set this to true. Otherwise, this may result in better performance if kept set to false.
        trackcount: false 
        
        # Ethernet header options.
        eth:
            # The source MAC address. If not set, the program will retrieve the MAC address of the interface we are binding to (the "interface" value).
            srcmac: NULL

            # The destination MAC address. If not set, the program will retrieve the default gateway's MAC address.
            dstmac: NULL
        
        # IP header options.
        ip:
            # Source ranges in CIDR format. By default, these aren't set, but I wanted to show an example anyways. These will be used if 'srcip' is not set.
            ranges:
                - 172.16.0.0/16
                - 10.60.0.0/24
                - 192.168.30.0/24
            
            # The source IPv4 address. If not set, you will need to specify source ranges in CIDR format like the above. If no source IP ranges are set, a warning will be outputted to `stderr` and 127.0.0.1 (localhost) will be used.
            srcip: NULL

            # The destination IPv4 address. If not set, the program will output an error. We require a value here. Otherwise, the program will shutdown.
            dstip: NULL

            # The IP protocol to use. At the moment, the only supported values are udp, tcp, and icmp.
            protocol: udp

            # The Type-Of-Service field (8-bit integer).
            tos: 0
            
            # The Time-To-Live field (8-bit integer). For static, set min and max to the same value.
            ttl:
                # Each packet generated will pick a random TTL. This is the minimum value within that range.
                min: 0

                # Each packet generated will pick a random TTL This is the maximum value within that range.
                max: 0
            
            # The ID field. For static, set min and max to the same value.
            id:
                # Each packet generated will pick a random ID. This is the minimum value within that range.
                min: 0

                # Each packet generated will pick a random ID. This is the maximum value within that range.
                max: 0

            # If true, we will calculate the IP header's checksum. If your NIC supports checksum offload with the IP header, disabling this option may improve performance within the program.
            csum: true

        # If true, we will calculate the layer-4 protocol checksum (UDP, TCP, and ICMP).
        l4csum: true

        # UDP header options.
        udp:
            # The source port. If 0, the program will generate a random number between 1 and 65535.
            srcport: 0

            # The destination port. If 0, the program will generate a random number between 1 and 65535.
            dstport: 0

        # TCP header options.
        tcp:
            # The source port. If 0, the program will generate a random number between 1 and 65535.
            srcport: 0

            # The destination port. If 0, the program will generate a random number between 1 and 65535.
            dstport: 0

            # If true, will set the TCP SYN flag.
            syn: false

            # If true, will set the TCP ACK flag.
            ack: false
        
            # If true, will set the TCP PSH flag.
            psh: false

            # If true, will set the TCP RST flag.
            rst: false

            # If true, will set the TCP FIN flag.
            fin: false

            # If true, will set the TCP URG flag.
            urg: false

            # If true, the socket will be setup as a cooked TCP socket. This establishes the three-way TCP handshake. WARNING - This makes the program ignore all of the headers. The only relevant information is the payload, destination IP, and port (must be static) when this is set to true.
            # NOTE - This is only supported for the standard version.
            usesocket: false

        # ICMP header options.
        icmp:
            # The code to use with the ICMP packet.
            code: 0

            # The type to use with the ICMP packet.
            type: 0

        # Payload options.
        payload:
            # Random payload generation/length.
            length:
                # The minimum payload length in bytes (payload is randomly generated).
                min: 0

                # The maximum payload length in bytes (payload is randomly generated).
                max: 0

            # If true, the application will only generate one payload per thread between the minimum and maximum lengths and generate the checksums once. In many cases, this will result in a huge performance gain because generating random payload per packet consumes a lot of CPU cycles depending on the payload length.
            isstatic: false

            # If true, the application will read data from the file 'exact' (below) is set to. The data within the file should be in the same format as the 'exact' setting without file support which is hexadecimal and separated by a space (e.g. "FF FF FF FF 59").
            isfile: false

            # If true, will parse the payload (either in 'exact' or the file within 'exact') as a string instead of hexadecimal.
            isstring: false

            # If a string, will set the payload to exactly this value. Each byte should be in hexadecimal and separated by a space. For example: "FF FF FF FF 59" (5 bytes of payload data).
            exact: NULL
```

As stated in the description, there are YAML config examples for Packet Batch [here](https://github.com/Packet-Batch/PB-Tests).

## Command Line Usage
There are a number of command line options available for Packet Batch. These are divided into basic command line usage and the first sequence override command line usage.

### Basic
Basic command line usage may be found below.

```bash
Usage: pcktbatch -c <configfile> [-v -h]

-c --cfg => Path to YAML file to parse.
-l --list => Print basic information about sequences.
-v --verbose => Provide verbose output.
-h --help => Print out help menu and exit program.
```

### First Sequence Override
If you wanted to quickly send packets and don't want to create a YAML config file, you may specify command line options to override the first sequence. You must also specify the `-z` or `--cli` flag in order to do this.

The following command line options are available to override the first sequence.

```bash
--interface => The interface to send out of.
--block => Whether to enable blocking mode (0/1).
--count => The maximum amount of packets supported.
--time => How many seconds to run the sequence for maximum.
--delay => The delay in-between sending packets on each thread.
--data => The maximum amount of data (in bytes) we can send.
--trackcount => Keep track of count regardless of it being 0 (read Configuration explanation for more information) (0/1).
--threads => The amount of threads and sockets to spawn (0 = CPU count).
--l4csum => Whether to calculate the layer-4 checksum (TCP, UDP, and ICMP) (0/1).

--srcmac => The ethernet source MAC address to use.
--dstmac => The ethernet destination MAC address to use.

--minttl => The minimum IP TTL to use.
--maxttl => The maximum IP TTL to use.
--minid => The minimum IP ID to use.
--maxid => The maximum IP ID to use.
--srcip => The source IP (one range is supported in CIDR format).
--dstip => The destination IP.
--protocol => The protocol to use (TCP, UDP, or ICMP).
--tos => The IP TOS to use.
--l3csum => Whether to calculate the IP header checksum or not (0/1).

--usrcport => The UDP source port.
--udstport => The UDP destination port.

--tsrcport => The TCP source port.
--tdstport => The TCP destination port.
--tsyn => Set the TCP SYN flag (0/1).
--tack => Set the TCP ACK flag (0/1).
--tpsh => Set the TCP PSH flag (0/1).
--trst => Set the TCP RST flag (0/1).
--tfin => Set the TCP FIN flag (0/1).
--turg => Set the TCP URG flag (0/1).
--tcpusesocket => Use TCP cooked socket (0/1).

--pmin => The minimum payload data.
--pmax => The maximum payload data.
--pstatic => Use static payload (0/1).
--pexact => The exact payload string.
--pfile => Whether to parse a file as the 'pexact' string instead.
--pstring => Parse the 'pexact' string or file as a string instead of hexadecimal.
```

## Building And Installing
While I suggest building and installing the application using the independent versions (standard, AF_XDP, or the DPDK), you may build and install this repository separately if you want as well. You may use `git` and `make`. When cloning the repository, you will want to use the `--recursive` flag to clone the needed submodule(s) as well (in this case, [libyaml](https://github.com/yaml/libyaml)). Otherwise, you will need to execute `git submodule update --init`.

The following should work.

```bash
# Clone the Packet Batch Common repository.
git clone --recursive https://github.com/Packet-Batch/PB-Common.git

# Install build essentials/tools and needed libaries for LibYAML.
sudo apt install build-essential clang autoconf libtool

# Change current working directory to PB-Common/.
cd PB-Common/

# Build and install (must be executed as root via sudo or root user due to libyaml installation).
sudo make

# Installing will copy default Packet Batch config to `/etc/pcktbatch/pcktbatch.yaml`.
sudo make install
```

## Credits
* [Christian Deacon](https://github.com/gamemann)
