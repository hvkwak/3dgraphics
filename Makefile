#
# Project 3d renderer
#
# @file
# @version 0.1s
#
CFLAGS += -Iinclude
build:
	gcc -Wall -std=c99 ${CFLAGS} ./src/*.c -lSDL2 -lm -lSDL2_image -o ./src/renderer

debug:
	gcc -Wall -std=c99 ${CFLAGS} -g -DDEBUG ./src/*.c -lSDL2 -lm -lSDL2_image -o ./src/renderer

run:
	./src/renderer

export:
	./src/renderer --export
	ffmpeg -i ./captures/frame_%04d.png -vf palettegen ./captures/palette.png
	ffmpeg -i ./captures/frame_%04d.png -i ./captures/palette.png -lavfi "fps=15,scale=320:-1:flags=lanczos[x];[x][1:v]paletteuse" ./output.gif

clean:
	rm ./src/renderer
# end
