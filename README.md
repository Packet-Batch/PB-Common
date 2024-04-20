# Packet Batch (Common)
[![Packet Batch Common Build Workflow](https://github.com/Packet-Batch/PB-Common/actions/workflows/build.yml/badge.svg)](https://github.com/Packet-Batch/PB-Common/actions/workflows/build.yml)

## Description
This is a repository for [Packet Batch](https://github.com/Packet-Batch) that includes common files for all versions ([Standard](https://github.com/Packet-Batch/PB-Standard), [AF_XDP](https://github.com/Packet-Batch/PB-AF-XDP), and [The DPDK](https://github.com/Packet-Batch/PB-DPDK)). This includes source files to parse YAML config files, command line parsing, and other useful functions.

## Dependencies
* [LibYAML](https://github.com/yaml/libyaml) - Used for parsing config files using the YAML syntax.

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

# Execute ./build.sh file to build and install dependencies and main project which requires sudo privileges.
# WARNING - If you don't have sudo available on your system, please look at the ./build.sh file and execute make commands as root in order.
# NOTE - You may also look at the .github/workflows/build.yml.
# NOTE - The first argument represents the amount of threads to use with make. 0 uses the amount of available threads on the system and supplying no argument uses 1 thread.
./build.sh 0

# (Not Required) If you want to clean our build, use the following. It must be ran as root or with sudo because we're also cleaning our LibYAML sub-module.
sudo make clean
```

## Credits
* [Christian Deacon](https://github.com/gamemann)
