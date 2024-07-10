SRC = $(wildcard *.c)
OBJ_DIR := ../../obj
OBJ := $(addprefix $(OBJ_DIR)/,$(SRC:.c=.o))

CFLAGS = -Wall -g -lpthread

all:$(OBJ)

$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:all
