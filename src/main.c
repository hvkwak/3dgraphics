#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"

////////////////////////////////////////////////////////////////////////////////
// Declare an array of vectors/points, variables, etc.                        //
////////////////////////////////////////////////////////////////////////////////
#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS]; // 9x9x9 cube
vec2_t projected_points[N_POINTS];
bool is_running = false;
float fov_factor = 640.0;
vec3_t camera_position = {.x = 0, .y = 0, .z = -5};

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

    int point_count = 0;
    // Statrt loading my array of vectors
    // From -1 to 1 (in this 9 * 9 * 9 cube)
    for (float x = -1; x <= 1.0; x += 0.25){
        for (float y = -1; y <= 1.0; y += 0.25){
            for (float z = -1; z <= 1.0; z += 0.25){
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count] = new_point;
                point_count++;
            }
        }
    }
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

////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point        //
////////////////////////////////////////////////////////////////////////////////
vec2_t project(vec3_t point){

    // projective projection
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z,
    };
    return projected_point;
}

void update(void){
    for (int i = 0; i < N_POINTS; i++){
        vec3_t point = cube_points[i];

        // Move the point away from the camera.
        point.z = point.z - camera_position.z;

        // project the curren tpoint
        vec2_t projected_point = project(point);

        // Save the projected 2D vectoro in the array of projected points
        projected_points[i] = projected_point;
    }
}

void render(void){

    /* Deprecated due to upcoming color buffers.
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Sets the drawing color to opaque red (R=255, G=0, B=0, A=255)
    SDL_RenderClear(renderer); // Clears the current rendering target (i.e., the screen or buffer) with the color set above → the screen will be filled with solid red.
    */

    clear_color_buffer(0xFF000000);


    // Loop all projected points and render them.
    for (int i = 0; i < N_POINTS; i++){
        vec2_t projected_point = projected_points[i];
        draw_rectangle(projected_point.x + window_width/2.0,
                       projected_point.y + window_height/2.0,
                       4,
                       4,
                       0xFFFFFF00
        );
    }

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
