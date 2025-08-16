#include "triangle.h"
#include "display.h"


/**
 * @brief renders "triangles_to_render"
 *
 * @param
 * @return
 */
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {

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
 * @brief swap two values!
 *
 * @param
 * @return
 */
void int_swap(int* a, int* b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

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
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
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
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
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
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){

    // Sort the verticies by y coordinates ascending.
    // Sort the projected points
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1){
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2){
        // simply draw flat bottom triangle.
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
        return;
    }else if (y0 == y1){
        // simply draw flat top triangle.
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }else{
        // find the Midpoint
        int Mx = (float)(x2 - x0) * (y1 - y0) / (float)(y2 - y0) + x0;
        int My = y1;

        // draw with flat bottom/top triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
    }
}
