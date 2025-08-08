#include "save.h"
#include <SDL2/SDL_stdinc.h>

int save_width = 0;
int save_height = 0;
Uint8* save_pixels = NULL;
int save_pitch = 0;
SDL_Surface *surface = NULL;

void flip_pixels_vertically(Uint8* pixels, int width, int height, int pitch) {
    Uint8* temp_row = (Uint8*)malloc(pitch);
    if (!temp_row) return;

    for (int y = 0; y < height / 2; ++y) {
        Uint8* row1 = pixels + y * pitch;
        Uint8* row2 = pixels + (height - 1 - y) * pitch;
        memcpy(temp_row, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, temp_row, pitch);
    }

    free(temp_row);
}
