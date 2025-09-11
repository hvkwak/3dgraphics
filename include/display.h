#ifndef DISPLAY_H
#define DISPLAY_H

#include "color.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>


#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS) // 1000 ms = 1 sec, depends on FPS

// render/cull mode enums
enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
};

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
};

// pipeline
bool initialize(void);
void render(void);
void update(void);
bool setup(void);

// getter and setters
int get_window_width(void);
int get_window_height(void);
SDL_Texture* get_SDL_Texture(void);
color_t* get_color_buffer(void);
float* get_z_buffer(void);
float get_delta_time(void);
void set_export(bool isExport);
void set_render_method(int render_method);
void set_cull_method(int cull_method);

// save
void flip_pixels_vertically(Uint8* pixels, int width, int height, int pitch);

// free and clears
void clear_color_buffer(color_t color);
void clear_z_buffer(void);
void destroy_display(void);
void destroy_save(void);


#endif // DISPLAY_H
