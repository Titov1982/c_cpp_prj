CC = gcc
CFLAGS = -O2 -g -std=gnu17 -Wall -Wextra \
				 -fsanitize=address

LD = gcc
LDFLAGS = -g

TARGET = main

MAIN_C = main.c darr.c
MAIN_H = darr.h
MAIN_O = $(MAIN_C: .c=.o)

all: $(TARGET)

include deps.make

main: $(MAIN_O)
	$(CC) $(CFLAGS) $^ -o $@

deps.make: $(MAIN_C) $(MAIN_H)
	$(CC) -MM $(MAIN_C) > deps.make




.PHONY: clean

clean:
	-rm -f *.o \
		*.gch $(TARGET) \
		deps.make
