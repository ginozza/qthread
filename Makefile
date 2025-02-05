CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
BUILD_DIR = build
SRC_DIR = src
EXAMPLES_DIR = examples

EXAMPLES_SRC = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLES = $(patsubst $(EXAMPLES_DIR)/%.c, $(BUILD_DIR)/%, $(EXAMPLES_SRC))

all: dirs $(BUILD_DIR)/qthread.o $(EXAMPLES)

dirs: 
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/qthread.o: $(SRC_DIR)/qthread.c include/qthread.h 
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c $(BUILD_DIR)/qthread.o 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean dirs
