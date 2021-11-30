# Use Clang to compile the common files.
CC = clang

# Directories.
BUILD_DIR := build
SRC_DIR := src
YAML_DIR := libyaml

# Source and out files.
UTILS_SRC := utils.c
UTILS_OUT := utils.o

CMD_LINE_SRC := cmd_line.c
CMD_LINE_OUT := cmd_line.o

CONFIG_SRC := config.c
CONFIG_OUT := config.o

# Global flags for optimization and telling the compiler we want object files.
GLOBAL_FLAGS := -O2 -c

# Chains.
all: libyaml mk_build utils cmd_line config

# Creates the build directory if it doesn't already exist.
mk_build:
	mkdir -p $(BUILD_DIR)

# LibYAML library we need to install.
libyaml:
	cd libyaml; ./bootstrap && ./configure
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

# Cleanup (remove object files and clean LibYAML).
clean:
	rm -f $(BUILD_DIR)/*.o
	$(MAKE) $(YAML_DIR)/ clean

.PHONY:

.DEFAULT: all