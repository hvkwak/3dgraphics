#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};
int previous_frame_time = 0;
bool is_running = false;
float fov_factor = 640.0;

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

    // Update the next frame if the difference between current and previous one
    // is bigger than target frame
    // -> using a awhile-loop could do it, but not recommendable.
    // while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

    // SDL_Delay
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks(); // Initiate after hitting SDL_INIT

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    // loop over all trinagle faces of the mesh
    for (int i = 0; i < N_MESH_FACES; i++){

        face_t mesh_face = mesh_faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1]; // index starts with 1. Has to be minus 1.
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++){
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate the vertex away from the camera
            transformed_vertex.z = transformed_vertex.z - camera_position.z;

            // Project the current vertex
            vec2_t projected_point = project(transformed_vertex);

            // Scale and translate the projected points to the middle of the screen
            projected_point.x = projected_point.x + (window_width/2.0);
            projected_point.y = -projected_point.y + (window_height/2.0);

            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles
        triangles_to_render[i] = projected_triangle;
    }

    /* for (int i = 0; i < N_POINTS; i++){ */
    /*     vec3_t point = cube_points[i]; */
    /*     vec3_t transformed_point = vec3_rotate_y(point, cube_rotation.y); */
    /*     transformed_point = vec3_rotate_x(transformed_point, cube_rotation.x); */
    /*     transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z); */

    /*     // Move the point away from the camera. */
    /*     transformed_point.z = transformed_point.z - camera_position.z; */

    /*     // project the curren tpoint */
    /*     vec2_t projected_point = project(transformed_point); */

    /*     // Save the projected 2D vectoro in the array of projected points */
    /*     projected_points[i] = projected_point; */
    /* } */
}

void render(void){

    clear_color_buffer(0xFF000000);

    // Loop all projected points and render them.
    for (int i = 0; i < N_MESH_FACES; i++){
        triangle_t triangle = triangles_to_render[i];
        draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
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
