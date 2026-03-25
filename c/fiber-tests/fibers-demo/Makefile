
CC = gcc
CFLAGS = -O2 -g -std=gnu17 -Wall -Wextra \
		  # -fsanitize=leak


%: %.c fibers.c fibers.h
	$(CC) $(CFLAGS) $^ -o $@



.PHONY:

clean:
	-rm -f *.o $@
