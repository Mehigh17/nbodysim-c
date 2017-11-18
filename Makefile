all:
	gcc main.c -lglfw

run: all 
	./a.out
