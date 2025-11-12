#include "display.h"
#include "triangle.h"
#include "camera.h"
#include "vector.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "clip.h"
#include "mesh.h"
#include "draw.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_image.h>

// Save Variables
static int save_width = 0;
static int save_height = 0;
static Uint8* save_pixels = NULL;
static int save_pitch = 0;
static SDL_Surface *save_surface = NULL;
static SDL_Texture *save_texture = NULL;
static bool is_export = false;
static int capture_idx = 0;
static int capture_max = 500;

// Display Variables
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static color_t* color_buffer = NULL;
static float* z_buffer = NULL;
static int window_width = 0;
static int window_height = 0;
static int previous_frame_time = 0;
static float delta_time;
static int render_method;
static int cull_method;

////////////////////////////////////////////////////////////////////////////////
// Getters and Setters
////////////////////////////////////////////////////////////////////////////////
SDL_Texture* get_SDL_Texture(void){
    return color_buffer_texture;
}

color_t* get_color_buffer(void){
    return color_buffer;
}

float* get_z_buffer(void){
    return z_buffer;
}

int get_window_height(void){
    return window_height;
}
int get_window_width(void){
    return window_width;
}

float get_delta_time(void){
    return delta_time;
}
void set_export(bool isExport){
    is_export = isExport;
}
void set_cull_method(int e){
    cull_method = e;
}

void set_render_method(int e){
    render_method = e;
}

////////////////////////////////////////////////////////////////////////////////
// Pipeline Functions
////////////////////////////////////////////////////////////////////////////////

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
    int full_screen_width = display_mode.w; // fake full-screen
    int full_screen_height = display_mode.h;

    // This will render color buffer in lower resolution
    window_width = full_screen_width / 3;
    window_height = full_screen_height / 3;

    if (window_height == 0 || window_width == 0){
        return false;
    }

    // Create a SDL Window - full screen
    window = SDL_CreateWindow(
        NULL, // no title
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        full_screen_width,
        full_screen_height,
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

    // initialize Saver
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
    return true;
}

/**
 * @brief setup the color buffer, color buffer texture, etc.
 *
 * @param
 * @return
 */
bool setup(void){

    // Initialize render mode and triangle culling method
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    // initialize projection matrix and frustum planes
    float znear = init_znear(1.0);
    float zfar = init_zfar(100.0);
    float aspect_x = init_aspect_x((float)get_window_width()/(float)get_window_height());
    float aspect_y = init_aspect_y((float)get_window_height()/(float)get_window_width());
    float fov_y = init_fovy(1.04716666667); // PI/3 = 60 degrees
    float fov_x = init_fovx(atan(tan(fov_y / 2) * aspect_x)*2.0);

    // build proj mat
    set_proj_mat(mat4_make_perspective(fov_y, aspect_y, znear, zfar));

    // initialilze frustum planes with a point and a normal
    init_frustum_planes(fov_x, fov_y, znear, zfar);

    // Load Multiple Meshes
    if (!load_mesh("../assets/f22.obj", "../assets/f22.png", vec3_new(1, 1, 1), vec3_new(-3, 0, 8), vec3_new(0.0, 0.0, 0.0))){
        return false;
    }
    if (!load_mesh("../assets/cube.obj", "../assets/cube.png", vec3_new(1, 1, 1), vec3_new(3, 0, 8), vec3_new(0.0, 0.0, 0.0))){
        return false;
    }
    // (...more meshes could be loaded.)

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void process_graphics_pipeline_stages(mesh_t* mesh){
    // Create the view matrix
    // initialize the target looking at the positive z-axis
    vec3_t target = {0, 0, 1};
    vec3_t up_direction = {0, 1, 0};
    set_target(&target); // rotate the camera direction
    set_view_mat(target, up_direction);

    // Create a scale, rotation, and translation mamtrix that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

    // loop over all trinagle faces of the mesh
    int num_faces = array_length(mesh->faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh->faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a];
        face_vertices[1] = mesh->vertices[mesh_face.b];
        face_vertices[2] = mesh->vertices[mesh_face.c];

        vec4_t transformed_vertices[3];
        // Loop all three vertices of this current face and apply
        // transformations
        for (int j = 0; j < 3; j++) {

            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Use a matrix to scale, translate, and rotate the original vertex
            mat4_t world_mat = mat4_identity();

            // Order matters: scale -> rotate -> translate
            world_mat = mat4_mul_mat4(scale_matrix, world_mat);
            world_mat = mat4_mul_mat4(rotation_matrix_x, world_mat);
            world_mat = mat4_mul_mat4(rotation_matrix_y, world_mat);
            world_mat = mat4_mul_mat4(rotation_matrix_z, world_mat);
            world_mat = mat4_mul_mat4(translation_matrix, world_mat);

            // multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_mat, transformed_vertex);

            // Multiply the view matrix by the vector to transform the scene to
            // camera space
            transformed_vertex = mat4_mul_vec4(get_view_mat(), transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Calculate the triangle face normal
        vec3_t vec_normal = get_triangle_normal(transformed_vertices);

        // Back-face Culling
        if (cull_method == CULL_BACKFACE) {

            // Find the vector between vectorA in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(vec3_from_vec4(transformed_vertices[0]), vec3_new(0, 0, 0)); // from A to camera position
            vec3_normalize(&camera_ray);

            // Calculate how aligned the camera ray is with the face normal (dot product)
            float cos_angle_normal_camera = vec3_dot(vec_normal, camera_ray);

            // Back face Culling, bypass triangles that are looking away from the camera.
            if (cos_angle_normal_camera > 0) { // invisible: beyond 90°
                continue;
            }
        }

        // Lighting
        light_t light = get_light();
        float cos_angle_normal_light = -vec3_dot(vec_normal, light.direction); // inverse
        color_t new_color = (color_t)light_apply_intensity(mesh_face.color, cos_angle_normal_light);

        // Create a polygon from the original transform
        polygon_t polygon = polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv);

        // clip the polygon and return a new polygon with potential new
        // vertices
        clip_polygon(&polygon);
        /* printf("Number of polygon vertices after clipping: %d\n",
         * polygon.num_vertices); */

        // Break the clipped polygon apart back into individual triangles
        triangle_t triangles_after_clipping[MAX_NUM_POLYGON_TRIANGLES];
        int num_traingles_after_clipping = 0;
        triangles_from_polygon(&polygon, triangles_after_clipping,
                               &num_traingles_after_clipping);

        // Loops all the assembled triangle after clipping
        for (int t = 0; t < num_traingles_after_clipping; t++) {

            triangle_t triangle_after_clipping = triangles_after_clipping[t];
            vec4_t projected_points[3];

            // Loop all three vertices to perform projection and conversion to
            // screen space.
            for (int j = 0; j < 3; j++) {

                // Project the current vertex
                projected_points[j] = mat4_mul_vec4_project(get_proj_mat(), triangle_after_clipping.points[j]);

                // Invert y values
                projected_points[j].y *= (-1.0);

                // Scale
                projected_points[j].x *= (float)window_width / 2.0;
                projected_points[j].y *= (float)window_height / 2.0;

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (float)window_width / 2.0;
                projected_points[j].y += (float)window_height / 2.0;
            }

            triangle_t triangle_to_render = {
            .points = {
            {projected_points[0].x, projected_points[0].y,projected_points[0].z, projected_points[0].w},
            {projected_points[1].x, projected_points[1].y,projected_points[1].z, projected_points[1].w},
            {projected_points[2].x, projected_points[2].y,projected_points[2].z, projected_points[2].w}},
            .textcoords = {
            {triangle_after_clipping.textcoords[0].u,triangle_after_clipping.textcoords[0].v},
            {triangle_after_clipping.textcoords[1].u,triangle_after_clipping.textcoords[1].v},
            {triangle_after_clipping.textcoords[2].u,triangle_after_clipping.textcoords[2].v}},
            .color = new_color,
            .texture = mesh->texture
        };

            // Save the projected triangle in the array of triangles
            int num_triangles_to_render = get_num_triangles_to_render();
            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                update_triangles_to_render(num_triangles_to_render, triangle_to_render);
                num_triangles_to_render++;
                set_num_triangles_to_render(num_triangles_to_render);
            }
        }
    }
}

/**
 * @brief updates the next frame
 *
 * @param
 * @return
 */
void update(void){

    // Note: This will control the FPS.
    // Wait some time until the it reaches the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Note: This controls the unit transformations per second, not per frame.
    // Get a delta time factor converted to seconds to be used to update our game objects
    // Note: having the delta-time ensures how many units I want to change per second, not per frame.
    delta_time = (SDL_GetTicks() - previous_frame_time)/1000.0;

    previous_frame_time = SDL_GetTicks(); // Initiate after hitting SDL_INIT

    // Initialize the counter of triangles to render for the current frame.
    set_num_triangles_to_render(0);

    for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++){
        mesh_t* mesh = get_mesh(mesh_index);

        // Change the mesh scale, rotation, and translation values per animation frame
        mesh->rotation.x += 0.6 * delta_time;
        mesh->rotation.y += 0.4 * delta_time;
        mesh->rotation.z += 0.8 * delta_time;
        /* mesh->translation.z += 0.0 * delta_time; */

        // Change the camera position per animation frame
        /* camera.position.x += 0.5*delta_time; */
        /* camera.position.y += 0.5*delta_time; */

        // Process the graphics pipeline stages foro every mesh of our 3D scene.
        process_graphics_pipeline_stages(mesh);
    }
}


/**
 * @brief render function in game loop. Note that it is triangle basis.
 *
 * @param
 * @return
 */
void render(void){

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    color_t* color_buffer = get_color_buffer();
    SDL_Texture* color_buffer_texture = get_SDL_Texture();

    // draw_grid(0xFFAAAAAA);

    // Loop all projected points and render them
    for (int i = 0; i < get_num_triangles_to_render(); i++) {

        // render all vertex points
        triangle_t triangle = get_triangle_to_render(i);

        // draw filled Triangle
        if (is_render_filled_triangle()){
			draw_filled_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
								 triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
								 triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
								 triangle.color, window_width, window_height, color_buffer, z_buffer); // dark gray
        }

        // draw textured triangle
        if (is_render_texture()){
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.textcoords[0],
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.textcoords[1],
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.textcoords[2],
                triangle.texture, window_width, window_height, color_buffer, z_buffer);
        }

        // draw wireframe
        if (is_render_wireframe()) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF, window_width, window_height, color_buffer);
        }

        // draw vertex in red
        if (render_method == RENDER_WIRE_VERTEX){
            draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF0000FF, window_width, window_height, color_buffer);
            draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF0000FF, window_width, window_height, color_buffer);
            draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF0000FF, window_width, window_height, color_buffer);
        }
    }

    // render_color_buffer();
    // color buffer -> color buffer texture
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(get_window_width()*sizeof(color_t))
    );

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
        snprintf(path, sizeof(path), "../captures/frame_%04d.png", ++capture_idx);
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
////////////////////////////////////////////////////////////////////////////////
// render options
////////////////////////////////////////////////////////////////////////////////
bool is_render_texture(void){
    return (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE);
}

bool is_render_filled_triangle(void){
    return (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE);
}

bool is_render_wireframe(void){
    return (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE);
}

////////////////////////////////////////////////////////////////////////////////
// frees and clears
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief frees color buffer, destroys the SDL window and its renderer.
 *
 * @param
 * @return
 */
void destroy_display(void){
    if (save_pixels != NULL){
        free(save_pixels);
    }
    if (color_buffer != NULL){
        free(color_buffer);
    }
    if (z_buffer != NULL){
        free(z_buffer);
    }
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
    for (int i = 0; i< window_height*window_width; i++){
        color_buffer[i] = color;
    }
}

void clear_z_buffer(void){
    for (int i = 0; i< window_height*window_width; i++){
        z_buffer[i] = 100.0f; // left-handed, [0.0, 1.0]
    }
}

void flip_pixels_vertically(Uint8* pixels, int width, int height, int pitch) {
    Uint8* temp_row = (Uint8*)malloc(pitch);
    if (!temp_row) return;

    for (int y = 0; y < height / 2; ++y) {
        Uint8* row1 = pixels + y * pitch;
        Uint8* row2 = pixels + (height - 1 - y) * pitch;
        memcpy(temp_row, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, temp_row, pitch);
    }
    free(temp_row);
}

/**
 * @brief frees color buffer, destroys the SDL window and its renderer.
 *
 * @param
 * @return
 */
void destroy_save(void){
    SDL_DestroyTexture(save_texture);
}
