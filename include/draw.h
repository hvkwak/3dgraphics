#ifndef DRAW_H
#define DRAW_H

#include "color.h"
#include "vector.h"
#include "texture.h"
#include "upng.h"

void draw_pixel(int x, int y, color_t color, int window_width, int window_height, color_t* color_buffer);
void draw_texel(int x, int y,
                vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t uv_a, tex2_t uv_b, tex2_t uv_c,
                upng_t* texture,
                int window_width, int window_height,
                color_t* color_buffer, float* z_buffer);
void draw_line(int x0, int y0, int x1, int y1, color_t color, int window_width, int window_height, color_t* color_buffer);
void draw_grid(color_t color, int window_width, int window_height, color_t* color_buffer);
void draw_rectangle(int x, int y, int w, int h, color_t color, int window_width, int window_height, color_t* color_buffer);


#endif // DRAW_H
