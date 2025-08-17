#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS) // 1000 ms = 1 sec, depends on FPS

// Declare a new type of definition to hold 32-bit colors value
typedef uint32_t color_t;


// declared here, but defined in .c -> extern
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern color_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize(void);
void destroy_objects(void);
void draw_grid(color_t color);
void draw_pixel(int x, int y, color_t color);
void draw_rectangle(int x, int y, int w, int h, color_t color);
void draw_line(int x0, int y0, int x1, int y1, color_t color);
void render_color_buffer(void);
void clear_color_buffer(color_t color);


#endif // DISPLAY_H
