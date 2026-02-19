CC = gcc
CFLAGS = -std=gnu17 -Wall -Werror -O2
LD = gcc
LDFLAGS = -g

TARGET = main

MAIN_C = main.c rand_interface.c gen_glibc.c
MAIN_H = rand_interface.h gen_glibc.h
MAIN_O = $(MAIN_C:.c=.o)




all : $(TARGET)

include deps.make

main : main.o rand_interface.o gen_glibc.o

deps.make : $(MAIN_C) $(MAIN_H) 
	gcc -MM $(MAIN_C) > deps.make



clean : 
	-rm -f $(TARGET) *.o
