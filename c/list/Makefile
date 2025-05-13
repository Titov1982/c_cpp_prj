

NAME_PRJ = list_test 
CC = gcc
CFLAGS = -c -g -Wall 
LDFLAGS = -o 


all: $(NAME_PRJ)

$(NAME_PRJ): main.o list.o
	$(CC) main.o list.o $(LDFLAGS) $(NAME_PRJ)


main.o: main.c
	$(CC) $(CFLAGS) main.c 

list.o: list.c
	$(CC) $(CFLAGS) list.c 


test_mem_leak:
	$(CC) -g -fsanitize=leak main.c list.c -o $(NAME_PRJ)

.PHONY test:
	./$(NAME_PRJ)

.PHONY clean:
	rm -rf *.o $(NAME_PRJ)
