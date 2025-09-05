#include "display.h"
#include "save.h"

// declared in display.h
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
color_t* color_buffer = NULL;
float* z_buffer = NULL;
int window_width = 0;
int window_height = 0;

/**
 * @brief initializes an SDL window an its renderer.
 *
 * @param
 * @return
 */
bool initialize(void){ // keep it void argument for no parameter.

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

/**
 * @brief frees color buffer, destroys the SDL window and its renderer.
 *
 * @param
 * @return
 */
void destroy_display(void){
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyTexture(save_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); // reverse of init.
}


/**
 * @brief clears color buffer with the color
 *
 * @param the color with which to clear the color buffer.
 * @return
 */
void clear_color_buffer(color_t color){
    for (int y = 0; y< window_height; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[(window_width*y) + x] = color;
        }
    }
}

void clear_z_buffer(void){
    for (int y = 0; y< window_height; y++){
        for (int x = 0; x < window_width; x++){
            z_buffer[(window_width*y) + x] = 100.0f; // left-handed, [0.0, 1.0]
        }
    }
}
