all:
	gcc main.c -lglfw -lm

run: all 
	./a.out
