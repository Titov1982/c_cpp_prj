
CC = gcc
CFLAGS = -O2 -g -std=gnu17 -Wall -Wextra \
				 # -fsanitize=leak
				 # -fsanitize=leak \
				 # -fsanitize=address

TARGET = cpy



$(TARGET): $(TARGET).o #darr.o

$(TARGET).o: $(TARGET).c #darr.c darr.h
	$(CC) $(CFLAGS) $(TARGET).c -c $(TARGET).o

# darr.o: darr.c
# 	$(CC) $(CFLAGS) darr.c -c darr.o

tmp:
	gcc -O2 -g -std=gnu11 -Wall -Wextra -fsanitize=leak cpy.c -o cpy


clean:
	-rm -f *.o cpy
