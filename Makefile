#
# Project 3dgraphics
# Note: run bear -- make -j all to get compile_commands.json
#
# @file
# @version 1.0
#
CFLAGS += -Iinclude

all: build build-tr

build:
	mkdir build
	gcc -Wall -std=c99 ${CFLAGS} ./src/*.c -lSDL2 -lm -lSDL2_image -o ./build/renderer

build-tr:
	mkdir build-tr
	gcc -Wall ./src-tr/*.c `sdl2-config --libs --cflags` -lSDL2 -lm -lSDL2_image -o ./build-tr/rasterizer

debug:
	mkdir build
	gcc -Wall -std=c99 ${CFLAGS} -g -DDEBUG ./src/*.c -lSDL2 -lm -lSDL2_image -o ./build/renderer

debug-tr:
	mkdir build-tr
	gcc -Wall -g -DDEBUG ./src-tr/*.c `sdl2-config --libs --cflags` -lSDL2 -lm -lSDL2_image -o ./build-tr/rasterizer

export:
	ffmpeg -i ./captures/frame_%04d.png -vf palettegen ./captures/palette.png
	ffmpeg -i ./captures/frame_%04d.png -i ./captures/palette.png -lavfi "fps=15,scale=640:-1:flags=lanczos[x];[x][1:v]paletteuse" ./output.gif

export-tr:
	ffmpeg -i ./captures-tr/frame_%04d.png -vf palettegen ./captures-tr/palette.png
	ffmpeg -i ./captures-tr/frame_%04d.png -i ./captures-tr/palette.png -lavfi "fps=15,scale=640:-1:flags=lanczos[x];[x][1:v]paletteuse" ./output-tr.gif

clean:
	rm -rf build
	rm -rf build-tr
# end
