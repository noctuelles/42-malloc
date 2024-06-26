# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/10 13:55:46 by plouvel           #+#    #+#              #
#    Updated: 2024/05/31 14:10:24 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Host type

ifeq ($(HOSTTYPE),)
	HOSTTYPE=$(shell uname -m)_$(shell uname -s)
endif

# Paths definition

SRC_PATH=src
INC_PATH=inc
TEST_PATH=test
BUILD_PATH=build
OBJS_PATH=$(BUILD_PATH)/objs
TEST_RESULTS_PATH=$(BUILD_PATH)/test_results

UNITY_PATH=unity/src

# Sources

SRC=block.c \
	orphean_block.c \
    heap.c \
	malloc.c \
	list.c \
	heap.c \
	tunable.c \
	utils.c \
	pool.c
SRC_TEST=$(wildcard $(TEST_PATH)/*.c)

# Compilation

CC=gcc
LINK=gcc

CFLAGS=-Wall -Werror -Wextra
DEBUG=-ggdb3

PROJECT_INC=-I$(INC_PATH) -I.
UNITY_INC=-I$(UNITY_PATH)
INCS=$(PROJECT_INC) $(UNITY_INC)

# Misc

RM=rm -rf
MKDIR=mkdir -p

PASSED = `grep -s PASS $(TEST_RESULTS_PATH)/*.txt`
FAIL = `grep -s FAIL $(TEST_RESULTS_PATH)/*.txt`
IGNORE = `grep -s IGNORE $(TEST_RESULTS_PATH)/*.txt`

# Outputs

PROJECT_OBJS=$(addprefix $(OBJS_PATH)/,$(SRC:.c=.o))
TEST_RESULTS=$(patsubst $(TEST_PATH)/test_%.c, $(TEST_RESULTS_PATH)/test_%.txt,$(SRC_TEST))

TARGET_EXTENSION=out
NAME=libft_malloc_$(HOSTTYPE).so

# Rules

all: $(NAME)

$(NAME): $(PROJECT_OBJS)
	$(LINK) -shared -o $@ $^

test: $(TEST_RESULTS_PATH) $(TEST_RESULTS)
	@echo "-----------------------\n\tIGNORED ⭕\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\n\tFAILURES ❌\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\n\tPASSED ✅\n-----------------------"
	@echo "$(PASSED)"

$(TEST_RESULTS_PATH)/%.txt: $(BUILD_PATH)/%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

# Rule to link test runner
$(BUILD_PATH)/test_%.$(TARGET_EXTENSION): $(OBJS_PATH)/test_%.o $(OBJS_PATH)/unity.o $(PROJECT_OBJS)
	$(LINK) -o $@ $^

# Rule to compile the files of the project
$(OBJS_PATH)/%.o:: $(SRC_PATH)/%.c | $(OBJS_PATH)
	$(CC) $(DEBUG) -fPIC $(CFLAGS) $(PROJECT_INC) -c $< -o $@

# Rule to compile the Unity Framework
$(OBJS_PATH)/%.o:: $(UNITY_PATH)/%.c $(UNITY_PATH)/%.h | $(OBJS_PATH)
	$(CC) $(UNITY_INC) -c $< -o $@

# Rule to compile the tests of the project
$(OBJS_PATH)/%.o:: $(TEST_PATH)/%.c | $(OBJS_PATH)
	$(CC) $(DEBUG) $(INCS) -c $< -o $@

$(OBJS_PATH):
	$(MKDIR) $@

$(TEST_RESULTS_PATH):
	$(MKDIR) $@

clean:
	$(RM) $(BUILD_PATH)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all  test clean fclean re
.PRECIOUS: $(BUILD_PATH)/test_%.$(TARGET_EXTENSION)
.PRECIOUS: $(OBJS_PATH)/%.o
.PRECIOUS: $(TEST_RESULTS_PATH)/%.txt