SRC := $(wildcard *.c)

OBJ_DIR := ../obj

OBJ := $(addprefix $(OBJ_DIR)/,$(SRC:.c=.o))

CFLAGS=-Wall -g -I ../include -I drivers

all:$(OBJ)

$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)/*.o

.PHONY:all  clean

