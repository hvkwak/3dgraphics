#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS) // 1000 ms = 1 sec, depends on FPS

// Declare a new type of definition to hold 32-bit colors value
typedef uint32_t color_t;

// declared here, but defined in .c -> extern.
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture* color_buffer_texture;
extern color_t* color_buffer;
extern float * z_buffer;
extern int window_width;
extern int window_height;

bool initialize(void);
void destroy_display(void);
void clear_color_buffer(color_t color);
void clear_z_buffer(void);


#endif // DISPLAY_H
