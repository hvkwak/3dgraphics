#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "save.h"
#include <stdlib.h>
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "camera.h"
#include "draw.h"

// render/cull mode enums: DO NOT move to other files, otherwise "multiple" definition error.
enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
} render_method;


bool is_running = true;
bool is_export = false;
int previous_frame_time = 0;
int capture_idx = 0;
int capture_max = 150;

/**
 * @brief setup the color buffer and color buffer texture
 *
 * @param
 * @return
 */
bool setup(void){

    // Initialize render mode and triangle culling method
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    // Allocate the required bytes in memory for the color buffer.
    color_buffer = (color_t*)malloc(sizeof(color_t) * window_width * window_height);
    if (color_buffer == NULL){
        return false;
    }

    // Allocate Z-buffer
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);
    if (z_buffer == NULL){
        return false;
    }

    // Create an SDL texture that is used to save
    // Allocate downsized pixel buffer (ARGB8888 = 4 bytes per pixel) for PNG export
    save_width  = window_width/2;
    save_height = window_height/2;
    save_pitch  = save_width * 4;  // ARGB8888 => 4 bytes per pixel

    // allocate save pixels
    save_pixels = (Uint8 *)malloc(save_pitch * save_height);
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
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    if (color_buffer_texture == NULL){
        return false;
    }

    proj_mat = mat4_make_perspective(fov, aspect, znear, zfar);

    // Manually load the hardcoded texture data from the static array
    /* mesh_texture = (uint32_t*)REDBRICK_TEXTURE; */

    // load the vertex and face values for the mesh data structure
    // load the texture information from an external PGN file
    // load_cube_mesh_data();

#ifdef DEBUG
    // For Debugging in Emacs
    char* png_filename = "../assets/f22.png";
    char* obj_filename = "../assets/f22.obj";
#else
    char* png_filename = "./assets/f22.png";
    char* obj_filename = "./assets/f22.obj";
#endif
    if (!load_obj_file_data(obj_filename)){
        return false;
    }
    if (!load_png_texture_data(png_filename)){
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
                break;
            }
            else if (event.key.keysym.sym == SDLK_1){
                // 1 Displays the wireframe and a small red dot for each triangle vertex
                render_method = RENDER_WIRE_VERTEX;
                break;
            }else if (event.key.keysym.sym == SDLK_2){
                // 2 Displays only the wireframe lines
                render_method = RENDER_WIRE;
                break;
            }else if (event.key.keysym.sym == SDLK_3){
                // 3 Displays filled triangles with a solid color
                render_method = RENDER_FILL_TRIANGLE;
                break;
            }else if (event.key.keysym.sym == SDLK_4){
                // 4 Displays both filled triangles and wireframe lines
                render_method = RENDER_FILL_TRIANGLE_WIRE;
                break;
            }else if (event.key.keysym.sym == SDLK_5){
                // 4 Displays textured face
                render_method = RENDER_TEXTURED;
                break;
            }else if (event.key.keysym.sym == SDLK_6){
                // 4 Displays textured face and wire
                render_method = RENDER_TEXTURED_WIRE;
                break;
            }else if (event.key.keysym.sym == SDLK_e){
                // E Enables back-face culling e
                cull_method = CULL_BACKFACE;
                break;
            }else if (event.key.keysym.sym == SDLK_d){
                // D Disables the back-face culling
                cull_method = CULL_NONE;
                break;
            }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point        //
////////////////////////////////////////////////////////////////////////////////


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

    // Initialize the counter of triangles to render for the current frame.
    num_traingles_to_render = 0;

    mesh.rotation.x += 0.01;
    /* mesh.rotation.y += 0.01; */
    /* mesh.rotation.z += 0.01; */
     /* mesh.scale.x += 0.002; */
    /* mesh.scale.y += 0.001; */
    /* mesh.translation.x += 0.01; */
    mesh.translation.z = 5.0; // Translate the vertex away from the camera(0, 0, 0)

    // Create a scale, rotation, and translation mamtrix that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // loop over all trinagle faces of the mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3];
        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++){

            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Use a matrix to scale, translate, and rotate the original vertex
            // Create a World Matrix combining scale, translatioon, and rotation
            mat4_t world_matrix = mat4_identity();

            // Order matters: scale -> rotate -> translate
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // normal vector for lighting
        vec3_t vec_normal;
        vec3_t vectorA = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vectorB = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vectorC = vec3_from_vec4(transformed_vertices[2]);

        vec3_t vec_AtoB = vec3_sub(vectorB, vectorA); // B-A
        vec3_t vec_AtoC = vec3_sub(vectorC, vectorA); // C-A
        vec3_normalize(&vec_AtoB);
        vec3_normalize(&vec_AtoC);

        vec_normal = vec3_cp(vec_AtoB, vec_AtoC);
        vec3_normalize(&vec_normal);

        vec3_t vec_camera = vec3_sub(camera_position, vectorA); // from A to camera position
        vec3_normalize(&vec_camera);

        float cos_angle_normal_camera = vec3_dot(vec_normal, vec_camera);

        // Back-face Culling
        if (cull_method == CULL_BACKFACE){
            if (cos_angle_normal_camera < 0) { // invisible: beyond 90°
              continue;
            }
        }

        // get the angle between light and normal -> color change due to light
        float cos_angle_normal_light = -vec3_dot(vec_normal, light.direction); // inverse
        color_t new_color = (color_t)light_apply_intensity(mesh_face.color, cos_angle_normal_light);

        vec4_t projected_points[3];
        // Project the face.
        for (int j = 0; j < 3; j++){

            // Project the current vertex
            projected_points[j] = mat4_mul_vec4_project(proj_mat, transformed_vertices[j]);

            // Invert y values
            projected_points[j].y *= (-1.0);

            // Scale
            projected_points[j].x *= (float)window_width / 2.0;
            projected_points[j].y *= (float)window_height / 2.0;

            // Translate the projected points to the middle of the screen
            projected_points[j].x += (float)window_width / 2.0;
            projected_points[j].y += (float)window_height / 2.0;
        }

        triangle_t projected_triangle = {
            .points = {{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                       {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                       {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}},
            .color = new_color,
            .textcoords = {
                {mesh_face.a_uv.u, mesh_face.a_uv.v},
                {mesh_face.b_uv.u, mesh_face.b_uv.v},
                {mesh_face.c_uv.u, mesh_face.c_uv.v}
            }
        };

        // Save the projected triangle in the array of triangles
        if (num_traingles_to_render < MAX_TRIANGLES_PER_MESH){
          triangles_to_render[num_traingles_to_render] = projected_triangle;
          num_traingles_to_render++;
        }
    }
}

void render(void){

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    // draw_grid(0xFFAAAAAA);

    // Loop all projected points and render them
    for (int i = 0; i < num_traingles_to_render; i++) {

        // render all vertex points
        triangle_t triangle = triangles_to_render[i];

        // draw filled Triangle
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){
			draw_filled_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
								 triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
								 triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
								 triangle.color); // dark gray
        }

        // draw textured triangle
        if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE){
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.textcoords[0],
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.textcoords[1],
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.textcoords[2],
                mesh_texture);
        }

        // draw wireframe
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF);
        }

        // draw vertex in red
        if (render_method == RENDER_WIRE_VERTEX){
            draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFF0000);
            draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFF0000);
            draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFF0000);
        }
    }

    // render_color_buffer();
    // color buffer -> color buffer texture
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width*sizeof(color_t))
    );

    // TODO: check if there's memory leak
    // Export in PNG
    if (is_export && SDL_GetTicks() > 3000 && capture_idx < capture_max){

        // Render full-res texture into small_rt at half size
        SDL_SetRenderTarget(renderer, save_texture);
        SDL_RenderClear(renderer);
        SDL_Rect dst = {0, 0, save_width, save_height};
        SDL_RenderCopy(renderer, color_buffer_texture, NULL, &dst);
        SDL_RenderFlush(renderer);

        // Read pixels from renderer: renderer -> save_pixels
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

        // Save to PNG
        char path[256];
#ifdef DEBUG
        snprintf(path, sizeof(path), "../captures/frame_%04d.png", ++capture_idx);
#else
        snprintf(path, sizeof(path), "./captures/frame_%04d.png", ++capture_idx);
#endif
        if (IMG_SavePNG(save_surface, path) == 0){
            printf("[cap] %s\n", path);
        }else{
            fprintf(stderr, "IMG_SavePNG failed: %s\n", SDL_GetError());
        }

        // Restore render target back to window
        SDL_SetRenderTarget(renderer, NULL);

        // Free save_surface
        SDL_FreeSurface(save_surface);
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
    if (png_texture != NULL){
        upng_free(png_texture);
    }
    if (save_pixels != NULL){
        free(save_pixels);
    }
    if (color_buffer != NULL){
        free(color_buffer);
    }
    if (z_buffer != NULL){
        free(z_buffer);
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
int main(int argc, char *argv[]){

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--export") == 0 || strcmp(argv[i], "-e") == 0) {
            is_export = true;
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
