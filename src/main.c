#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

bool is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width;
int window_height;

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

void setup(void){
    // Allocate the required bytes in memory for the color buffer.
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    // Create an SDL texture that is used to display the color buffer.
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

}

void process_input(void){
    // read event
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
            }
            break;
    }
}

void update(void){
    // TODO
}

void draw_grid(uint32_t color){
    // Exercise: Drawing a Background Grid
    // Draw a background grid that fills the entire window.
    // Lines should be rendered at every row/col multiple of 10.
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

void render(void){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Sets the drawing color to opaque red (R=255, G=0, B=0, A=255).
    SDL_RenderClear(renderer); // Clears the current rendering target (i.e., the screen or buffer) with the color set above → the screen will be filled with solid red.

    clear_color_buffer(0xFFFFFF00);
    draw_grid(0x00000000);
    render_color_buffer();

    SDL_RenderPresent(renderer); // Displays the result on the window.
}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); // reverse of init.
}

int main(void){

    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}
