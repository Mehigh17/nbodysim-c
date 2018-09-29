all:
	gcc -o simulation src/*.c -lm -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
