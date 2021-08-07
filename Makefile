TARGET_EXEC := bench

CC := cc
CFLAGS := -std=c11
CFLAGS += -Og -march=native
CFLAGS += -ggdb
CFLAGS += -Wall -Wextra -Wconversion -Wpedantic
CFLAGS += -MMD -MP
LDFLAGS := -ldl

SRC_DIR = src
BUILD_DIR = build

SRCS = $(shell find $(SRC_DIR) -name *.c)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.SUFFIXES:

.PHONY: clean

-include $(DEPS)
