CC = gcc
CFLAGS = -O2 -g -std=gnu17 -Wall -Wextra \
				 -fsanitize=address

LD = ld


main: main.o darr.o darr.h
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c darr.h
	$(CC) $(CFLAGS) $^ -c $@

darr.o: darr.c darr.h
	$(CC) $(CFLAGS) $^ -c $@

# darr.o: darr.c darr.h
# 	$(CC) $(CFLAGS) $^ -c $@
#
# %.o: %.c darr.h
# 	$(CC) $(CFLAGS) $^ -c $@
#
# %: %.o darr.o darr.h
# 	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm -f *.o \
		*.gch main
