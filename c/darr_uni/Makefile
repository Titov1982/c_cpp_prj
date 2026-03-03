CC = gcc
# CFLAGS = -O2 -g -std=gnu17 -Wall -Wextra -fsanitize=address
CFLAGS = -O2 -std=gnu17 -Wall -Wextra

LD = gcc
LDFLAGS = -g -L .
LDLIBS = -ldarr

TARGET = main

MAIN_C = main.c darr.c
MAIN_H = darr.h
MAIN_O = $(MAIN_C: .c=.o)




all: $(TARGET)

include deps.make

# main: $(MAIN_O)
# 	$(CC) $(CFLAGS) $^ -o $@
main: $(MAIN_O) libdarr.a
	$(LD) $(LDFLAGS) $(MAIN_O) $(LDLIBS) -o $@

libdarr.a: darr.o
	$(AR) rvf $@ $^

deps.make: $(MAIN_C) $(MAIN_H)
	$(CC) -MM $(MAIN_C) > deps.make




.PHONY: clean

clean:
	-rm -f *.o \
		*.a \
		*.gch $(TARGET) \
		deps.make
