#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"
#include <stdint.h>
#include "display.h"

typedef struct {
    int a;
    int b;
    int c;
    color_t color;
} face_t;

typedef struct {
    vec2_t points[3];
    color_t color;
    float avg_depth;
} triangle_t;

bool swap_triangle(triangle_t* a, triangle_t* b);
int compare_triangle(const void * a, const void * b);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, color_t color);

#endif // TRIANGLE_H
