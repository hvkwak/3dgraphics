#include "triangle.h"
#include "display.h"
#include "swap.h"
#include "draw.h"

// Array of triangles that should be rendered frame by frame
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_traingles_to_render;


/**
 * @brief swaps the two triangles
 *
 * @param pointer to triangle a and b
 * @return
 */
bool swap_triangle(triangle_t* pa, triangle_t* pb){
    if (compare_triangle(pa, pb)){
        triangle_t tmp = *pa;
        *pa = *pb;
        *pb = tmp;
        return true;
    }
    return false;
}

/**
 * @brief compares the depth of the two triangels.
 *
 * @param two pointers to triangles a and b
 * @return true, if a.avg_depth < b.avg_depth
 */
bool compare_triangle(const void * a, const void * b){
    const triangle_t* A = (const triangle_t*)a;
    const triangle_t* B = (const triangle_t*)b;
    return (A->avg_depth < B->avg_depth);
}

/**
 * @brief renders "triangles_to_render"
 *
 * @param
 * @return
 */
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {

    // dots
    draw_rectangle(x0, y0, 3, 3, color);
    draw_rectangle(x1, y1, 3, 3, color);
    draw_rectangle(x2, y2, 3, 3, color);

    // draw all edges (wireframes)
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

/**
 * @brief draws textured triangle
 *
 * @param x, y: coordinates of vertex
 *        z: depth of vertex
 *        w: original depth of vertex
 *        u, v: texcoords of vertex
 * @return
 */
void draw_textured_triangle(int x0, int y0, float z0, float w0, tex2_t uv_a,
                            int x1, int y1, float z1, float w1, tex2_t uv_b,
                            int x2, int y2, float z2, float w2, tex2_t uv_c,
                            uint32_t* texture){
    // sort the vertices by y-coordinates ascending. (y0 < y1 < y2)
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&uv_a.u, &uv_b.u);
        float_swap(&uv_a.v, &uv_b.v);
    }
    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&uv_b.u, &uv_c.u);
        float_swap(&uv_b.v, &uv_c.v);
    }
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&uv_a.u, &uv_b.u);
        float_swap(&uv_a.v, &uv_b.v);
    }

    // Flip the V component to account for inverted uv_coordinates
    // (here we should have v grows downwards, .obj is upward.)
    uv_a.v = 1.0 - uv_a.v;
    uv_b.v = 1.0 - uv_b.v;
    uv_c.v = 1.0 - uv_c.v;

    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    /////////////////////////////////////////////////////////
    // render the upper part of the triangle (flat-bottom) //
    /////////////////////////////////////////////////////////
    float inv_slope1 = 0; // left. Note that it is dx/dy.
    float inv_slope2 = 0; // right.

    if (y1 - y0 != 0) inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0){
      for (int y = y0; y <= y1; y++) {// starting from top to y1: upper part of the triangle

        int x_start = x1 + (y - y1) * inv_slope1;
        int x_end = x0 + (y - y0) * inv_slope2;

        if (x_end < x_start) {
          // swap if x_start is to the right of x_end
          int_swap(&x_start, &x_end);
        }

        // draw_line() doesn't work here. We go pixel-by-pixel
        for (int x = x_start; x < x_end; x++) {
          // draw with the color from the texture
          draw_texel(x, y, point_a, point_b, point_c, uv_a, uv_b, uv_c, texture);
        }
      }
    }

    /////////////////////////////////////////////////////////
    // render the lower part of the triangle (flat-top)    //
    /////////////////////////////////////////////////////////
    inv_slope1 = 0; // left. Note that it is dx/dy.
    inv_slope2 = 0; // right.

    if (y2 - y1 != 0) inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0){
      for (int y = y1; y <= y2; y++) {// starting from top to y1: upper part of the triangle
        int x_start = x1 + (y - y1) * inv_slope1;
        int x_end = x0 + (y - y0) * inv_slope2;

        if (x_end < x_start) {
          // swap if x_start is to the right of x_end
          int_swap(&x_start, &x_end);
        }

        // draw_line() doesn't work here. We go pixel-by-pixel
        for (int x = x_start; x < x_end; x++) {
          // draw with the color from the texture
          draw_texel(x, y, point_a, point_b, point_c, uv_a, uv_b, uv_c, texture);
        }
      }
    }
};

/**
 * @brief fills flat bottom triangle.
 *
 * @param triangle coordiantes
 * @return
 *
 *                   P0 (x0, y0)
 *                   /\
 *                  /  \
 *          slope1 /    \ slope2
 *                /      \
 *    (x1, y1) P1 -------- (x2, y2)
 */
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color){
    // find two slopes: two triangle legs.
    // Note that we find slop with respect to delta y.
    float inv_slope1 = (float)(x1-x0)/(y1-y0);
    float inv_slope2 = (float)(x2-x0)/(y2-y0);

    // Start x_start and x_end from the top vertex (x0, y0)
    float x_start = x0;
    float x_end = x0;

    // Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; y++){
        draw_line((int)x_start, y, (int)x_end, y, color);
        x_start += inv_slope1;
        x_end += inv_slope2;
    }
}


/**
 * @brief fills flat top triangle.
 *
 * @param triangle coordiantes
 * @return
 *
 *
 *   (x0, y0) P1 -------- (x1, y1)
 *              \_       \
 *                \_      \
 *                  \_     \
 *                    \_    \
 *                      \_   \
 *                        \_  \
 *                          \_ \
 *                            \_\ P2 (x2, y2)
 *
 *
 *
 */
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color){
    // Draw flat-top triangle
    // Note that we find slop with respect to delta y.
    float inv_slope1 = (float)(x2-x0)/(y2-y0); // left leg
    float inv_slope2 = (float)(x2-x1)/(y2-y1); // right leg

    // Start x_start and x_end from the top vertex (x0, y0)
    float x_start = x2;
    float x_end = x2;

    // Loop all the scanlines from top to bottom
    for (int y = y2; y >= y0; y--){
        draw_line((int)x_start, y, (int)x_end, y, color);
        x_start -= inv_slope1;
        x_end -= inv_slope2;
    }
};

/**
 * @brief draws filled triangle based on flat-top, flat-bottom triangle.
 *                  P0 (x0, y0)
 *                 /\
 *                /  \
 *               /    \
 *              /      \
 *  (x1, y1) P1 -------- (Mx, My)
 *              \_       \
 *                \_      \
 *                  \_     \
 *                    \_    \
 *                      \_   \
 *                        \_  \
 *                          \_ \
 *                            \_\ P2 (x2, y2)
 *
 * @param x, y coordinates of 3 triangle points
 *        color: to fille
 * @return
 */
/* void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color){ */

/*     // Sort the verticies by y coordinates ascending. */
/*     // Sort the projected points */
/*     if (y0 > y1) { */
/*       int_swap(&y0, &y1); */
/*       int_swap(&x0, &x1); */
/*     } */
/*     if (y1 > y2) { */
/*       int_swap(&y1, &y2); */
/*       int_swap(&x1, &x2); */
/*     } */
/*     if (y0 > y1) { */
/*       int_swap(&y0, &y1); */
/*       int_swap(&x0, &x1); */
/*     } */

/*     if (y1 == y2) { */
/*       // simply draw flat bottom triangle. */
/*       fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color); */
/*       return; */
/*     } else if (y0 == y1) { */
/*       // simply draw flat top triangle. */
/*       fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color); */
/*     } else { */
/*       // find the Midpoint */
/*       int Mx = (float)(x2 - x0) * (y1 - y0) / (float)(y2 - y0) + x0; */
/*       int My = y1; */

/*       // draw with flat bottom/top triangle */
/*       fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color); */
/*       fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color); */
/*     } */
/* } */

void draw_filled_triangle(int x0, int y0, float z0, float w0,
						  int x1, int y1, float z1, float w1,
						  int x2, int y2, float z2, float w2,
						  color_t color){
	// sort the vertices by y-coordinates ascending. (y0 < y1 < y2)
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    /////////////////////////////////////////////////////////
    // render the upper part of the triangle (flat-bottom) //
    /////////////////////////////////////////////////////////
    float inv_slope1 = 0; // left. Note that it is dx/dy.
    float inv_slope2 = 0; // right.

    if (y1 - y0 != 0) inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0){
      for (int y = y0; y <= y1; y++) {// starting from top to y1: upper part of the triangle

        int x_start = x1 + (y - y1) * inv_slope1;
        int x_end = x0 + (y - y0) * inv_slope2;

        if (x_end < x_start) {
          // swap if x_start is to the right of x_end
          int_swap(&x_start, &x_end);
        }

        // draw_line() doesn't work here. We go pixel-by-pixel
        for (int x = x_start; x < x_end; x++) {
          // draw with the color from the texture
          draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
        }
      }
    }

    /////////////////////////////////////////////////////////
    // render the lower part of the triangle (flat-top)    //
    /////////////////////////////////////////////////////////
    inv_slope1 = 0; // left. Note that it is dx/dy.
    inv_slope2 = 0; // right.

    if (y2 - y1 != 0) inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0){
      for (int y = y1; y <= y2; y++) {// starting from top to y1: upper part of the triangle
        int x_start = x1 + (y - y1) * inv_slope1;
        int x_end = x0 + (y - y0) * inv_slope2;

        if (x_end < x_start) {
          // swap if x_start is to the right of x_end
          int_swap(&x_start, &x_end);
        }

        // draw_line() doesn't work here. We go pixel-by-pixel
        for (int x = x_start; x < x_end; x++) {
          // draw with the color from the texture
          draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
        }
      }
    }
}
