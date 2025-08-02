#include "display.h"


// declared in display.h
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height =600;

bool initialize_window(void){ // keep it void argument for no parameter.

    // Check if the SDL library initialization works
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what's the fullscreen max. widith and height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode); // monitor 0.
    window_width = display_mode.w; // fake full-screen
    window_height = display_mode.h;

    if (window_height == 0 || window_width == 0){
        return false;
    }

    // Create a SDL Window
    window = SDL_CreateWindow(
        NULL, // no title
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window){
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0); // no special flags. 0.
    if (!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}


void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); // reverse of init.
}

/// Exercise:
/// Draw a background grid that fills the entire window.
/// Lines should be rendered at every row/col multiple of 10.
/**
 * @brief draws a background grid
 *
 * @param color: color of grid pixels
 * @return
 */

void draw_grid(uint32_t color){

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
 * @brief draws a pixel
 * @param x: x pos of pixel
 *        y: y pos of pixel
 *        color: color of pixel
 * @return
 */
void draw_pixel(int x, int y, uint32_t color){

    if (0 <= x && x < window_width && 0 <= y && y < window_height){
        color_buffer[window_width*y + x] = color;
    }
}


/// Exercise: Create a functiton called draw_rect() that renders a rectangle on
///           the screen.
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
void draw_rectangle(int x, int y, int w, int h, uint32_t color){

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
                draw_pixel(x+i, y+j, color); // use draw_pixel()!
                //color_buffer[window_width*(y+j) + (x + i)] = color;
            }
        }
    }
}

void render_color_buffer(void){
    // color buffer -> color buffer texture -> display texture
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width*sizeof(uint32_t))
    );

    // color buffer texture -> display texture
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color){
    for (int y = 0; y< window_height; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[(window_width*y) + x] = color;
        }
    }
}
