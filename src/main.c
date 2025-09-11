#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "mesh.h"
#include "camera.h"

static bool is_running = true;

/**
 * @brief process keyboard input while running.
 *
 * @param
 * @return
 */
void process_input(void){
    // read event
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                } else if (event.key.keysym.sym == SDLK_1) {
                    // 1 Displays the wireframe and a small red dot for each triangle vertex
                    set_render_method(RENDER_WIRE_VERTEX);
                } else if (event.key.keysym.sym == SDLK_2) {
                    // 2 Displays only the wireframe lines
                    set_render_method(RENDER_WIRE);
                } else if (event.key.keysym.sym == SDLK_3) {
                    // 3 Displays filled triangles with a solid color
                    set_render_method(RENDER_FILL_TRIANGLE);
                } else if (event.key.keysym.sym == SDLK_4) {
                    // 4 Displays both filled triangles and wireframe lines
                    set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                } else if (event.key.keysym.sym == SDLK_5) {
                    // 4 Displays textured face
                    set_render_method(RENDER_TEXTURED);
                } else if (event.key.keysym.sym == SDLK_6) {
                    // 4 Displays textured face and wire
                    set_render_method(RENDER_TEXTURED_WIRE);
                } else if (event.key.keysym.sym == SDLK_b) {
                    // b Enables back-face culling
                    set_cull_method(CULL_BACKFACE);
                } else if (event.key.keysym.sym == SDLK_f) {
                    // f Disables the back-face culling
                    set_cull_method(CULL_NONE);
                } else if (event.key.keysym.sym == SDLK_d) {
                    // d rotate camera yaw +
                    set_camera_yaw(1.0 * get_delta_time());
                } else if (event.key.keysym.sym == SDLK_a) {
                    // a rotate camera yaw -
                    set_camera_yaw(-1.0 * get_delta_time());
                } else if (event.key.keysym.sym == SDLK_w) {
                    // w moves camera forward
                    float delta_time = get_delta_time();
                    set_camera_forward(delta_time);
                } else if (event.key.keysym.sym == SDLK_s) {
                    // s moves camera backward
                    float delta_time = get_delta_time();
                    set_camera_forward(-delta_time);
                } else if (event.key.keysym.sym == SDLK_UP) {
                    // up translates camera position +
                    float delta_time = get_delta_time();
                    set_camera_translate(-3.0 * delta_time);
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    // down translates camera position -
                    float delta_time = get_delta_time();
                    set_camera_translate(3.0 * delta_time);
                }
                break;
        }
    };
}

/**
 * @brief frees the malloc memory 
 *
 * @param
 * @return
 */
void free_resources(void){
    free_mesh();
    free_texture();
}

/**
 * @brief main function
 *
 * @param
 * @return
 */
int main(int argc, char *argv[]){

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--export") == 0 || strcmp(argv[i], "-e") == 0) {
            set_export(true);
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            fprintf(stderr, "Usage: %s [--export]\n", argv[0]);
            return 1;
        }
    }

	if (!initialize()){
		printf("initialization() failed");
		return 1;
	};

	if (!setup()){
		printf("Setup() failed");
		return 1;
	}

    while (is_running) {
        process_input();
        update();
        render();
    }

	destroy_save();
    destroy_display();
    free_resources();
    return 0;
}
