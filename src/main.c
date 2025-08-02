#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"

bool is_running = false;

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

void render(void){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Sets the drawing color to opaque red (R=255, G=0, B=0, A=255).
    SDL_RenderClear(renderer); // Clears the current rendering target (i.e., the screen or buffer) with the color set above → the screen will be filled with solid red.

    clear_color_buffer(0xFF000000);
    draw_grid(0xFF333300);
    draw_rectangle(1820, 980, 200, 300, 0xFFFFFF00);
    render_color_buffer();

    SDL_RenderPresent(renderer); // Displays the result on the window.
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
