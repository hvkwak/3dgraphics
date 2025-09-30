#include "color.h"
#include "vector.h"
#include "texture.h"
#include "util.h"
#include <stdlib.h>
#include <math.h>
#include "upng.h"

/**
 * @brief draws a pixel
 * @param x: x pos of pixel
 *        y: y pos of pixel
 *        color: color of pixel
 * @return
 */
void draw_pixel(int x, int y, color_t color, int window_width, int window_height, color_t* color_buffer){

    if (x < 0){
        return;
    }
    if (window_width <= x){
        return;
    }
    if (y < 0){
        return;
    }
    if (window_height <= y){
        return;
    }
    color_buffer[window_width*y + x] = color;
}

/// Exercise: Draw a background grid that fills the entire window. Lines should be rendered at every row/col multiple of 10.
/**
 * @brief draws a background grid
 *
 * @param color: color of grid pixels
 * @return
 */
void draw_grid(color_t color, int window_width, int window_height, color_t* color_buffer){

    int nx = window_width/10;
    int ny = window_height/10;

    // horizontal lines
    for (int y = 1; y < ny; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[window_width*y*10 + x] = color;
         }
    }

    // vertical lines
    for (int x = 1; x < nx; x++){
        for (int y = 0; y < window_height; y++){
            color_buffer[x*10 + window_width * y] = color;
        }
    }
}

/**
 * @brief implements DDA Algorithm to draw a line between two points.
 *
 * @param int x0: x coordinate of P0
 *        int y0: y coordinate of P0
 *        int x1: x coordinate of P1
 *        int y1: y coordinate of P1
 *        color_t : color of line points
 * @return
 */
void draw_line(int x0, int y0, int x1, int y1, color_t color, int window_width, int window_height, color_t* color_buffer){
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x): abs(delta_y);

    // Find how much we should increment in both x and y each step
    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++){
        draw_pixel(round(current_x), round(current_y), color, window_width, window_height, color_buffer);
        current_x += x_inc;
        current_y += y_inc;
    }
}


/// Exercise: Create a functiton called draw_rect() that renders a rectangle on the screen.
/**
* @brief draws rectangle
*
* @param
* x: x pos of top left
* y: y pos of top left
* w: width of the rectangle
* y: height of the rectangle
* @return
*/
void draw_rectangle(int x, int y, int w, int h, color_t color, int window_width, int window_height, color_t* color_buffer){

    // check arguments
    if (x < 0 || x >= window_width) {
        return;
    }
    if (y < 0 || y >= window_height) {
        return;
    }
    if (w <= 0 || w > window_width){
        return;
    }
    if (h <= 0 || h > window_height){
        return;
    }

    // Update Color Buffer
    // e.g. 600 x 400
    // -> [0, 599], [0, 399]
    //
    // x+i : current_x
    // y+j : current_y
    // i stays the innermost one!
    for(int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){
            if ((y + j) < window_height && (x + i) < window_width){
                draw_pixel(x+i, y+j, color, window_width, window_height, color_buffer); // use draw_pixel()!
            }
        }
    }
}

/**
 * @brief draws texture at the coordinate
 *
 * @param x, y                      - coordinates of point to render
 *        point_a, point_b, point_c - vertices of the triangle
 *        uv_a, uv_b, uv_c          - texture coordinates of vertices
 *        texture                   - pointer to texture
 * @return
 */
void draw_texel(int x, int y,
                vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t uv_a, tex2_t uv_b, tex2_t uv_c,
                upng_t* texture,
                int window_width, int window_height,
                color_t* color_buffer, float* z_buffer)
{
    vec2_t p = {x, y}; // point
    vec2_t a = vec2_from_vec4(point_a); // take the first two coordinates
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Note 1: Potential misunderstanding due to wording(?)
    // Perspective is non-linear transform that involves dividing by w.
    // -> geometry gets distorted in screen space.
    // u/w, v/w, and 1/w, however, vary linearly and safe to work with barycentric weights.

    // Note 2: Remember this: x_p = x/w
    // x_p has a non-linear relationship with w,
    // x_p has a linear relationship with 1/w

    // Note3 : interpolation of all u/w and v/w values using barycentric weights and a factor of 1/w
    // 1. alpha, beta, and gamma are based on the 2D projected points
    // 2. But texture coordinates u, v belong to the original 3D world/texture space. Not projected yet!
    // 3. interplation need to be based on u/w and v/w values.
    interpolated_u = (uv_a.u/point_a.w)*alpha + (uv_b.u/point_b.w)*beta + (uv_c.u/point_c.w)*gamma;
    interpolated_v = (uv_a.v/point_a.w)*alpha + (uv_b.v/point_b.w)*beta + (uv_c.v/point_c.w)*gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1/point_a.w)*alpha + (1/point_b.w)*beta + (1/point_c.w)*gamma;

    // divide back both interpolated values by 1/w
    // back to normal u, v coordinates to restore the “true” perspective-correct texture coordinates!
    // Note: Misunderstood? it is recommended then to recall when we draw pixel P(x, y) of the exact vertex
    //       position, where e.g. alpha = 1.0, beta = 0.0, gamma = 0.0.
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Get the mesh texture width and height dimensions
    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);

    // map the uv coordinate to the full texture width and height
    // module due to "truncation error"
    int tex_x = abs((int)(interpolated_u * texture_width))%texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height))%texture_height;


    /* if (texture_width <= tex_x){ */
    /*     tex_x = texture_width - 1; */
    /* } */
    /* if (texture_height <= tex_y){ */
    /*     tex_y = texture_height -1; */
    /* } */

    /* // Draw a pixel at position (x, y) with the color that comes from the mapped */
    /* // texture */
    /* //draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]); */

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;
    /* interpolated_reciprocal_w = 1/interpolated_reciprocal_w; */

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer.
    if (interpolated_reciprocal_w < z_buffer[(window_width * y) + x]){

        // Get the buffer of colors from the texture
        uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);

        // Draw a pixel at position (x, y) with the color that comes from the mapped texture
        draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x], window_width, window_height, color_buffer);

        // Update the z-buffer value with the 1/w of this current pixel.
        z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
    }
}
