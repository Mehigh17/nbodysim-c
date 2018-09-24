all:
	gcc src/*.c -lm -lSDL2 -lSDL2_image -lSDL2_ttf

run: all 
	./a.out
