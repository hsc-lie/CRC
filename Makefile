CC=gcc


DEBUG?=1

BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
OUT_DIR=$(BUILD_DIR)/out

Target=$(OUT_DIR)/crc_test


OPT=-O0


SRCS=main.c\
     crc/crc.c\
	 crc/crc_table.c\


INCLUDE_DIR=-I crc\


OBJS=$(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))


CFLAG+=$(OPT) $(INCLUDE_DIR) -Wall -MMD -MP

ifeq ($(DEBUG),1)
    CFLAG+= -g
endif

vpath %c crc

$(Target):$(OBJS) | $(OUT_DIR)
	$(CC) $(CFLAG) $^ -o $@

$(OBJ_DIR)/%.o:%.c Makefile | $(OBJ_DIR)
	$(CC) -c $(CFLAG) $< -o $@


$(OBJ_DIR):$(BUILD_DIR)
	mkdir -p $@

$(OUT_DIR):$(BUILD_DIR)
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

-include $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.d)))

.PHONY:clear printf


clear:
	rm -rf $(BUILD_DIR)

printf:
	echo $(OBJS)

