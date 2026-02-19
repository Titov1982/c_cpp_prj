
CC = gcc
CFLAGS = -O2 -g -std=gnu11 -Wall -Wextra -fsanitize=leak


%: %.c 
	$(CC) $(CFLAGS) $^ -o $@



.PHONY:

clean:
	-rm -f *.o
