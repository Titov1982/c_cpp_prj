CC = gcc
CFLAGS = -std=gnu17 -Wall -Werror -O2 
LD = gcc
LDFLAGS = -g -L .
LDLIBS = -lgen_glibc

TARGET = main

MAIN_C = main.c rand_interface.c 
MAIN_H = rand_interface.h gen_glibc.h
MAIN_O = $(MAIN_C:.c=.o)




all : $(TARGET)

include deps.make

main : $(MAIN_O) libgen_glibc.a
	$(LD) $(LDFLAGS) $(MAIN_O) $(LDLIBS) -o $@

libgen_glibc.a : gen_glibc.o
	$(AR) rvf $@ $^

deps.make : $(MAIN_C) $(MAIN_H) 
	gcc -MM $(MAIN_C) > deps.make



clean : 
	-rm -f $(TARGET) *.o *.a

