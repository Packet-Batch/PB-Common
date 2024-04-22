# Use Clang to compile the common files.
CC = clang

# Package config.
PKG_CONF = pkg-config

ifeq ($(shell which $(PKG_CONF)),)
$(error "Package config not found. Please install it on server.")
endif

# Directories.
BUILD_DIR := build
SRC_DIR := src
MODULES_DIR := modules
DATA_DIR := data
JSONC_DIR := $(MODULES_DIR)/json-c
TESTS_DIR := tests

# Source and out files.
UTILS_SRC := utils.c
UTILS_OUT := utils.o

CMD_LINE_SRC := cmd_line.c
CMD_LINE_OUT := cmd_line.o

CONFIG_SRC := config.c
CONFIG_OUT := config.o

# Config file.
CONFIG_EX := conf.json

# Global flags for optimization and telling the compiler we want object files.
GLOBAL_FLAGS := -O2 -c

# Chains.
all: mk_build utils cmd_line config

# Creates the build directory if it doesn't already exist.
mk_build:
	mkdir -p $(BUILD_DIR)

# LibYAML library we need to install.
jsonc:
	mkdir -p $(JSONC_DIR)/build
	cd $(JSONC_DIR)/build && cmake ../

jsonc_install:
	cd $(JSONC_DIR)/build && make && make install

# The utils file.
utils: mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(UTILS_OUT) $(SRC_DIR)/$(UTILS_SRC)

# The command line file.
cmd_line: mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CMD_LINE_OUT) $(SRC_DIR)/$(CMD_LINE_SRC)

# The config file.
config: mk_build
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CONFIG_OUT) $(SRC_DIR)/$(CONFIG_SRC)

custom_tests:
	$(CC) -O2 -g -I src/ $(shell $(PKG_CONF) --libs json-c) $(BUILD_DIR)/$(CMD_LINE_OUT) $(BUILD_DIR)/$(CONFIG_OUT) -o $(BUILD_DIR)/test_cfg_print $(TESTS_DIR)/cfg_print.c

# Install (copy base config file if it doesn't already exist).
install:
	mkdir -p /etc/pcktbatch
	cp -n $(DATA_DIR)/$(CONFIG_EX) /etc/pcktbatch/$(CONFIG_EX)

# Cleanup (remove object files and clean LibYAML).
clean:
	rm -f $(BUILD_DIR)/*.o
	rm -rf $(JSONC_DIR)/build/*

.PHONY:

.DEFAULT: all