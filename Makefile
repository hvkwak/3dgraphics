#
# Project 3d renderer
#
# @file
# @version 0.1s
#
CFLAGS += -Iinclude
build:
	gcc -Wall -std=c99 ${CFLAGS} ./src/*.c -lSDL2 -lm -o ./src/renderer

debug:
	gcc -Wall -std=c99 ${CFLAGS} -g ./src/*.c -lSDL2 -lm -o ./src/renderer

run:
	./src/renderer

clean:
	rm ./src/renderer
# end
