MAKEFLAGS += --no-print-directory

BUILD_TYPE ?= debug

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests
THIRD_PARTY_DIR := third_party

BIN := $(BUILD_DIR)/meruem_$(BUILD_TYPE)
TEST_BIN := $(BUILD_DIR)/test_meruem_$(BUILD_TYPE)

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

LIB_OBJS := $(filter-out $(BUILD_DIR)/main.o,$(OBJS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))
UNITY_OBJ := $(BUILD_DIR)/unity.o
TEST_DEPS := $(TEST_OBJS:.o=.d) $(UNITY_OBJ:.o=.d)

all: $(BIN)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/test_%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -I$(THIRD_PARTY_DIR)/unity/src $(CFLAGS) -c $< -o $@

$(UNITY_OBJ): $(THIRD_PARTY_DIR)/unity/src/unity.c
	@mkdir -p $(dir $@)
	$(CC) -I$(THIRD_PARTY_DIR)/unity/src $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJS) $(UNITY_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

compile_commands: clean
	bear -- $(MAKE) $(BIN) $(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS) $(TEST_DEPS)

.PHONY: all test compile_commands clean
