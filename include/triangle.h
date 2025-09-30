#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdbool.h>
#include "vector.h"
#include "texture.h"
#include "color.h"
#include "upng.h"

#define MAX_TRIANGLES_PER_MESH 10000

typedef struct {
    int a; // vertex numbers
    int b;
    int c;
    tex2_t a_uv; // texture uv coordinates
    tex2_t b_uv;
    tex2_t c_uv;
    color_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t textcoords[3];
    color_t color;
    upng_t* texture;
} triangle_t; // triangle for rendering

void update_triangles_to_render(int i, triangle_t triangle);
triangle_t get_triangle_to_render(int i);
void set_num_triangles_to_render(int num);
int get_num_triangles_to_render(void);
vec3_t get_triangle_normal(vec4_t vertices[3]);

void draw_triangle_pixel(int x, int y,
                         vec4_t point_a, vec4_t point_b, vec4_t point_c,
                         color_t color,
                         int window_width, int window_height,
                         color_t* color_buffer, float* z_buffer);
void draw_triangle(int x0, int y0,
                   int x1, int y1,
                   int x2, int y2,
                   color_t color,
                   int window_width, int window_height, color_t* color_buffer);
void draw_filled_triangle(int x0, int y0, float z0, float w0,
                          int x1, int y1, float z1, float w1,
                          int x2, int y2, float z2, float w2,
                          color_t color,
                          int window_width, int window_height,
                          color_t* color_buffer, float* z_buffer);
void draw_textured_triangle(int x0, int y0, float z0, float w0, tex2_t uv_a,
                            int x1, int y1, float z1, float w1, tex2_t uv_b,
                            int x2, int y2, float z2, float w2, tex2_t uv_c,
                            upng_t* texture, int window_width, int window_height, color_t* color_buffer, float* z_buffer);
void fill_flat_bottom_triangle(int x0, int y0,
                               int x1, int y1,
                               int x2, int y2,
                               color_t color, int window_width, int window_height, color_t* color_buffer);
void fill_flat_top_triangle(int x1, int y1,
                            int Mx, int My,
                            int x2, int y2,
                            color_t color, int window_width, int window_height, color_t* color_buffer);

#endif // TRIANGLE_H
