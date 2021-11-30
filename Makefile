CC = clang

BUILD_DIR := build
SRC_DIR := src
YAML_DIR := libyaml

UTILS_SRC := utils.c
UTILS_OUT := utils.o

CMD_LINE_SRC := cmd_line.c
CMD_LINE_OUT := cmd_line.o

CONFIG_SRC := config.c
CONFIG_OUT := config.o

GLOBAL_FLAGS := -O2 -c

all: libyaml mkbuild utils cmd_line config

mkbuild:
	mkdir -p $(BUILD_DIR)

libyaml:
	cd libyaml; ./bootstrap && ./configure
	$(MAKE) -C $(YAML_DIR)/
	$(MAKE) -C $(YAML_DIR)/ install

utils: mkbuild
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(UTILS_OUT) $(SRC_DIR)/$(UTILS_SRC)

cmd_line: mkbuild
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CMD_LINE_OUT) $(SRC_DIR)/$(CMD_LINE_SRC)

config: libyaml mkbuild
	$(CC) $(GLOBAL_FLAGS) -o $(BUILD_DIR)/$(CONFIG_OUT) $(SRC_DIR)/$(CONFIG_SRC)

clean:
	rm -f $(BUILD_DIR)/*.o
	$(MAKE) $(YAML_DIR)/ clean

.PHONY:

.DEFAULT: all