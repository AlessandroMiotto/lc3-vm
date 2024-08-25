CC = gcc
FLAGS = -Wall -Wextra

main: main.c lc3vm.c
	$(CC) main.c lc3vm.c -o main $(FLAGS)

run:
	@./main

clean:
	@rm main
