TARGET_EXEC := bench

CC := cc
CFLAGS := -std=gnu11
CFLAGS += -Og -march=native
CFLAGS += -ggdb
CFLAGS += -Wall -Wextra -Wconversion
CFLAGS += -MMD -MP
LDFLAGS := -ldl

SRC_DIR = src
BUILD_DIR = build

SRCS = $(shell find $(SRC_DIR) -name *.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.SUFFIXES:

.PHONY: clean

-include $(DEPS)
