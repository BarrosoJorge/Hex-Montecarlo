CC = gcc
CFLAGS = -Wall -O2 -I.

SRCS = main.c Board/board.c Board/square.c Player/player.c Rules/rules.c Utils/utils.c pcg_basic.c
OBJS = $(SRCS:.c=.o)
TARGET = hex

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)