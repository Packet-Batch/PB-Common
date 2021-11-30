# Use Clang to compile the common files.
CC = clang

# Directories.
BUILD_DIR := build
SRC_DIR := src
MODULES_DIR := modules
DATA_DIR := data

YAML_DIR := $(MODULES_DIR)/libyaml

# Source and out files.
UTILS_SRC := utils.c
UTILS_OUT := utils.o

CMD_LINE_SRC := cmd_line.c
CMD_LINE_OUT := cmd_line.o

CONFIG_SRC := config.c
CONFIG_OUT := config.o

# Config file.
CONFIG_EX := pcktbatch.yaml

# Global flags for optimization and telling the compiler we want object files.
GLOBAL_FLAGS := -O2 -c

# Chains.
all: libyaml mk_build utils cmd_line config

# Creates the build directory if it doesn't already exist.
mk_build:
	mkdir -p $(BUILD_DIR)

# LibYAML library we need to install.
libyaml:
	cd $(YAML_DIR)/; ./bootstrap && ./configure
	$(MAKE) -C $(YAML_DIR)/
	$(MAKE) -C $(YAML_DIR)/ install

# The utils file.
utils: mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(UTILS_OUT) $(SRC_DIR)/$(UTILS_SRC)

# The command line file.
cmd_line: mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CMD_LINE_OUT) $(SRC_DIR)/$(CMD_LINE_SRC)

# The config file.
config: libyaml mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CONFIG_OUT) $(SRC_DIR)/$(CONFIG_SRC)

# Install (copy base config file if it doesn't already exist).
install:
	mkdir -p /etc/pcktbatch
	cp -n $(DATA_DIR)/$(CONFIG_EX) /etc/pcktbatch/$(CONFIG_EX)

# Cleanup (remove object files and clean LibYAML).
clean:
	rm -f $(BUILD_DIR)/*.o

.PHONY:

.DEFAULT: all