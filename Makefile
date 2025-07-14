BUILD_TYPE ?= debug

BUILD_DIR := build
SRC_DIR := src
BIN := $(BUILD_DIR)/meruem-$(BUILD_TYPE)

CC := gcc
CPPFLAGS := -I$(SRC_DIR) -MMD -MP
CFLAGS := -std=c23 -Wall -Wextra -Werror -Wpedantic -Wconversion

ifeq ($(BUILD_TYPE),release)
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS := -flto
else
	CFLAGS += -g3
	LDFLAGS := -fsanitize=address,undefined
endif

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

.PHONY: all clean
