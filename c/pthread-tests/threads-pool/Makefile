all:
	gcc -std=c11 -g -O0 -fsanitize=leak -o threads-pool main.c tai_thread_pool.c -lpthread

optim:
	gcc -std=c11 -O3 -o threads-pool main.c -lpthread
