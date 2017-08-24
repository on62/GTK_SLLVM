NAME = eco
CFLAGS = -g -Wall -o $(NAME)
GTKFLAGS = -export-dynamic `pkg-config --cflags --libs gtk+-3.0`
SRCS = main.c allocate.c arrays.c draw.c sim.c window.c
CC = gcc

# top-level create main
all: main

# compile source
main: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(GTKFLAGS)

# clean
clean:
	rm -fv $(NAME)
