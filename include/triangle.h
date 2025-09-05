#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"
#include "texture.h"
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
	float avg_depth;
} triangle_t; // triangle for rendering

extern triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
extern int num_traingles_to_render;

bool swap_triangle(triangle_t* a, triangle_t* b);
bool compare_triangle(const void * a, const void * b);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
void draw_filled_triangle(int x0, int y0, float z0, float w0,
							int x1, int y1, float z1, float w1,
							int x2, int y2, float z2, float w2,
							color_t color);
void draw_textured_triangle(int x0, int y0, float z0, float w0, tex2_t uv_a,
                            int x1, int y1, float z1, float w1, tex2_t uv_b,
                            int x2, int y2, float z2, float w2, tex2_t uv_c,
                            uint32_t* texture);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, color_t color);

#endif // TRIANGLE_H
