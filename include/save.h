#ifndef SAVE_H
#define SAVE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

extern int save_width;
extern int save_height;
extern Uint8* save_pixels;
extern int save_pitch;
extern SDL_Surface *save_surface;
extern SDL_Texture *save_texture;

void flip_pixels_vertically(Uint8* pixels, int width, int height, int pitch);

#endif // SAVE_H
