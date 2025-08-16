#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS) // 1000 ms = 1 sec, depends on FPS

// declared here, but defined in .c -> extern
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;

//
extern bool is_backface;
extern bool is_wireframe;
extern bool is_redvertex;
extern bool is_filledtriangle;

bool initialize(void);
void destroy_objects(void);
void draw_grid(uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rectangle(int x, int y, int w, int h, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);


#endif // DISPLAY_H
