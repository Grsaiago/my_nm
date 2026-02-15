CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c99
INCLUDES = -I./include/
TEST_INCLUDES = -lcriterion

SRC_DIR = src
OBJ_DIR = objs
TEST_DIR = tests
TEST_OBJ_DIR = $(OBJ_DIR)/tests

NAME = my_nm
TEST_NAME = test_$(NAME)

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))
LIB_OBJS = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

.PHONY: help
help: ## Show this help message
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TEST_OBJ_DIR):
	mkdir -p $(TEST_OBJ_DIR)

# Compile test files
$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build test binary
$(TEST_NAME): $(LIB_OBJS) $(TEST_OBJS)
	$(CC) $(LIB_OBJS) $(TEST_OBJS) -o $(TEST_NAME) $(INCLUDES) $(TEST_INCLUDES)

.PHONY: build
build: $(NAME) ## Build the project (default)

.PHONY: run
run: $(NAME) ## Compile and run the my_nm without args
	./$(NAME)

.PHONY: test
test: $(TEST_NAME) ## Build and run tests with criterion
	./$(TEST_NAME)

.PHONY: clean
clean: ## Remove build artifacts
	rm -rf $(OBJ_DIR) $(NAME) $(TEST_NAME)

.PHONY: re
re: clean all ## Clean and rebuild everything
