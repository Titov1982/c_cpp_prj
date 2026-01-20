all:
	gcc -std=c99 -g -O0 main.c -o thread-simple-test-2

test_mem_leak:
	gcc -std=c99 -g -fsanitize=leak main.c -o thread-simple-test-2

.PHONY clear:
	rm -rf thread-simple-test-2
