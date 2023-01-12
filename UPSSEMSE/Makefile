MKDIR   := mkdir
RMDIR   := rm -rf
CC      := gcc
BIN     := ./bin
OBJ     := ./obj
SRC     := ./src
SRCS    := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
EXEC    := $(BIN)/UPSSEMSE
CFLAGS  := -Wall -Wextra

.PHONY: all run clean

all: $(EXEC) 

$(EXEC): $(OBJS) | $(BIN)
	$(CC) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
	$(MKDIR) $@

run: $(EXEC) | clrscr
	$<

clean:
	$(RMDIR) $(OBJ) $(BIN)

clrscr:
	clear
