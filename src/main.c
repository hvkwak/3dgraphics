#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "save.h"

// Array of triangles that should be rendered frame by frame
triangle_t* triangles_to_render = NULL;

// other global variables
vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
int previous_frame_time = 0;
bool is_running = false;
bool is_outcome_produced = false;
float fov_factor = 640.0;


/**
 * @brief renders "triangles_to_render"
 *
 * @param
 * @return
 */
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // TODO: move it to triangle.c!
    draw_rectangle(x0, y0, 3, 3, color);
    draw_rectangle(x1, y1, 3, 3, color);
    draw_rectangle(x2, y2, 3, 3, color);

    // draw all edges
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

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

    // Allocate downsized pixel buffer (ARGB8888 = 4 bytes per pixel) for BMP export
    save_width  = window_width/2;
    save_height = window_height/2;
    save_pitch  = save_width * 4;  // ARGB8888 => 4 bytes per pixel
    save_pixels = (Uint8*)malloc(save_pitch * save_height);
    if (!save_pixels) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    // Better downscale quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // "1" linear, "2" best available

    save_texture = SDL_CreateTexture(renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_TARGET,
                                 save_width, save_height);
    if (!save_texture) {
        fprintf(stderr, "CreateTexture failed: %s\n", SDL_GetError());
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
    if (!load_obj_file_data("./assets/cube.obj")){
        return false;
    }
    return true;
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

    mesh.rotation.x += 0.03;
    mesh.rotation.y += 0.03;
    mesh.rotation.z += 0.03;

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
            face_vertices[j] = vec3_rotate_y(face_vertices[j], mesh.rotation.y);
            face_vertices[j] = vec3_rotate_x(face_vertices[j], mesh.rotation.x);
            face_vertices[j] = vec3_rotate_z(face_vertices[j], mesh.rotation.z);

            // Translate the vertex away from the camera(0, 0, 0)
            face_vertices[j].z = face_vertices[j].z + 5;
        }

        // Back-face Culling
        vec3_t vec_AtoB = vec3_sub(face_vertices[1], face_vertices[0]); // B-A
        vec3_normalize(&vec_AtoB);
        vec3_t vec_AtoC = vec3_sub(face_vertices[2], face_vertices[0]); // C-A
        vec3_normalize(&vec_AtoC);
        vec3_t vec_normal = vec3_cp(vec_AtoB, vec_AtoC);
        vec3_normalize(&vec_normal);

        vec3_t vec_camera = vec3_sub(camera_position, face_vertices[0]); // from A to camera position
        vec3_normalize(&vec_camera);

        float angle = vec3_dot(vec_normal, vec_camera);
        bool isVisible = angle > 0.0 ? true : false;
        if (!isVisible){
            continue;
        }

        // It is visible. Project the face.
        for (int j = 0; j < 3; j++){

            // Project the current vertex
            vec2_t projected_point = project(face_vertices[j]);

            // Save the projected 2D Vector in the array of projected triangle.
            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles
        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void){

    clear_color_buffer(0xFF000000);

    //draw_grid(0xFFAAAAAA);

    // Loop all projected points and render them
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        // render all vertex points
        triangle_t triangle = triangles_to_render[i];

        // Draw filled triangle
        draw_filled_triangle(triangle.points[0].x,
                             triangle.points[0].y,
                             triangle.points[1].x,
                             triangle.points[1].y,
                             triangle.points[2].x,
                             triangle.points[2].y,
                             0xFFFFFFFF);

        // Draw unfilled triangle
        draw_triangle(triangle.points[0].x,
                      triangle.points[0].y,
                      triangle.points[1].x,
                      triangle.points[1].y,
                      triangle.points[2].x,
                      triangle.points[2].y,
                      0xFF000000);
    }



    // Clear the array of triangles to render every frame loop
    array_free(triangles_to_render);

    // render_color_buffer();
    // color buffer -> color buffer texture
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width*sizeof(uint32_t))
    );


    if (!is_outcome_produced && SDL_GetTicks() > 2500){

        // Render full-res texture into small_rt at half size
        SDL_SetRenderTarget(renderer, save_texture);
        SDL_RenderClear(renderer);
        SDL_Rect dst = {0, 0, save_width, save_height};
        SDL_RenderCopy(renderer, color_buffer_texture, NULL, &dst);
        SDL_RenderFlush(renderer);

        // Read pixels from renderer
        if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, save_pixels, save_pitch) != 0) {
            fprintf(stderr, "SDL_RenderReadPixels failed: %s\n", SDL_GetError());
            return;
        }

        // Wrap pixel data in SDL_Surface
        save_surface = SDL_CreateRGBSurfaceFrom(save_pixels,
                                                save_width,
                                                save_height,
                                                32,
                                                save_pitch,
                                                0x00FF0000, // R
                                                0x0000FF00, // G
                                                0x000000FF, // B
                                                0xFF000000  // A
        );

        if (!save_surface) {
            fprintf(stderr, "SDL_CreateRGBSurfaceFrom failed: %s\n", SDL_GetError());
            return;
        }

        // Save to BMP
        if (SDL_SaveBMP(save_surface, "outcome.bmp") == 0) {
            printf("Screenshot saved as outcome.bmp\n");
            is_outcome_produced = true;
        } else {
            fprintf(stderr, "SDL_SaveBMP failed: %s\n", SDL_GetError());
        }
        // Restore render target back to window
        SDL_SetRenderTarget(renderer, NULL);
    }
    // color buffer texture -> display texture
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer); // Displays the result on the window.
}

/**
 * @brief frees the malloc memory 
 *
 * @param
 * @return
 */
void free_resources(void){
    if (save_pixels != NULL){
        free(save_pixels);
    }
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

    is_running = initialize() && setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_objects();
    free_resources();
    return 0;
}
