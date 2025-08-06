#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

// Array of triangles that should be rendered frame by frame
triangle_t* triangles_to_render = NULL;

// other global variables
vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
int previous_frame_time = 0;
bool is_running = false;
float fov_factor = 640.0;

/**
 * @brief setup the color buffer and color buffer texture
 *
 * @param
 * @return
 */
bool setup(void){
    // Allocate the required bytes in memory for the color buffer.
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    if (color_buffer == NULL){
        return false;
    }

    // Create an SDL texture that is used to display the color buffer.
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    if (color_buffer_texture == NULL){
        return false;
    }

    // Load the cube values in the mesh data structure
    // load_cube_mesh_data();
    if (load_obj_file_data("../assets/cube.obj")){
        return true;
    }
    return false;
}

/**
 * @brief process keyboard input while running.
 *
 * @param
 * @return
 */
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

    // perspective projection
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = -(fov_factor * point.y) / point.z,
    };

    // Scale and translate the projected points to the middle of the screen
    projected_point.x = projected_point.x + (window_width / 2.0);
    projected_point.y = -projected_point.y + (window_height / 2.0);

    return projected_point;
}

/**
 * @brief updates the next frame
 *
 * @param
 * @return
 */
void update(void){

    // Wait some time until the it reaches the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks(); // Initiate after hitting SDL_INIT

    // Initialize the array of triangles to render
    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    // loop over all trinagle faces of the mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++){

        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1]; // index starts with 1. Has to be minus 1.
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++){
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // Translate the vertex away from the camera
            transformed_vertex.z = transformed_vertex.z - camera_position.z;

            // Project the current vertex
            vec2_t projected_point = project(transformed_vertex);

            // Save the projected 2D Vector in the array of projected triangle.
            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles
        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void){

    clear_color_buffer(0xFF000000);

    // Loop all projected points and render them.
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++){

        // render all vertex points
        triangle_t triangle = triangles_to_render[i];
        draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        // draw all edges
        draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, 0xFFFFFF00);
        draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFF00);
        draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x, triangle.points[0].y, 0xFFFFFF00);
    }

    // Clear the array of triangles to render every frame loop
    array_free(triangles_to_render);

    render_color_buffer();
    SDL_RenderPresent(renderer); // Displays the result on the window.
}

/**
 * @brief frees the memory that was dynamically allocated by the program
 *
 * @param
 * @return
 */
void free_resources(void){
    if (color_buffer != NULL){
        free(color_buffer);
    }
    if (array_length(mesh.vertices) != 0){
        array_free(mesh.vertices);
    }
    if (array_length(mesh.faces) != 0){
        array_free(mesh.faces);
    }
}

/**
 * @brief main function
 *
 * @param
 * @return
 */
int main(void){

    is_running = initialize_window();

    if (!setup()){
        destroy_window();
        free_resources();
        return 0;
    };

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();
    return 0;
}
