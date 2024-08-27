CC = gcc
FLAGS = -O3

main: vm/main.c vm/lc3vm.c
	@$(CC) vm/main.c vm/lc3vm.c -o vm/main $(FLAGS)
	@python3 assembler/assembler.py

run:
	@./vm/main

clean:
	@rm vm/main
