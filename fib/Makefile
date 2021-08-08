TARGET_NAME := fib

CC := cc
CFLAGS := -std=gnu11
CFLAGS += -O2 -march=native -flto
CFLAGS += -Wall -Wextra -Wconversion
LDFLAGS :=

RUSTFLAGS := -C opt-level=3 -C target-cpu=native -C lto

SRCS := $(wildcard $(TARGET_NAME).*)
ALL_SHARED_LIBRARIES := $(SRCS:$(TARGET_NAME).%=lib$(TARGET_NAME)_%.so)

all: $(ALL_SHARED_LIBRARIES)

# TODO: PGO
lib$(TARGET_NAME)_c.so: $(TARGET_NAME).c
	$(CC) -shared -o $@ $< $(CFLAGS) $(LDFLAGS)

# TODO: PGO
lib$(TARGET_NAME)_rs.so: $(TARGET_NAME).rs
	rustc --crate-type cdylib -o $@ $< $(RUSTFLAGS)

clean:
	rm -f *.so

.SUFFIXES:

.PHONY: clean
