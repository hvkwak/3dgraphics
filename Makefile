#
# Project 3d renderer
#
# @file
# @version 0.1s
build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -o ./src/renderer

debug:
	gcc -Wall -std=c99 -g ./src/*.c -lSDL2 -o ./src/renderer

run:
	./renderer

clean:
	rm renderer
# end
