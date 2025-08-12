#include "triangle.h"

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int Mx, int My){
    // TODO: Draw flat-bottom triangle
};

void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2){
    // TODO: Draw flat-top triangle
};

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
    triangle_t sorted_triangle; // lowest y first.

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

    // find the Midpoint
    int Mx = (float)(x2 - x0)*(y1 - y0)/(float)(y2 - y0) + x0;
    int My = y1;

    // draw with flat bottom/top triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My);
    fill_flat_top_triangle(x1, y1, Mx, My, x2, y2);
}
