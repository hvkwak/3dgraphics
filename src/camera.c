#include "camera.h"
#include "matrix.h"

// Projection matrix parameters
static float fov_x; // PI/3 = 60 degrees
static float fov_y; // PI/3 = 60 degrees
static float aspect_x; // PI/3 = 60 degrees
static float aspect_y;
static float znear; // not too small, due to properties of NDC coordinates!
static float zfar;

// other global variables: camera pos, proj_mat, look_mat, ...
static mat4_t proj_mat; // projection matrix
static mat4_t view_mat; // look_at matrix
static mat4_t world_mat;

camera_t camera = {
    .position = {0, 0, 0},
    .direction = {0, 0, 1}, // looking at unit position in z, forward.
    .forward_velocity = {0, 0, 0},
    .yaw = 0.0
};

////////////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////////////
void set_camera_yaw(float delta){
    camera.yaw += delta;
}

void set_camera_direction(vec3_t target){
    camera.direction = vec3_rotate_y(target, camera.yaw);
}

void set_target(vec3_t* target){
    camera.direction = vec3_rotate_y(*target, camera.yaw);
    *target = vec3_add(camera.position, camera.direction);
}

void set_view_mat(vec3_t target, vec3_t up_direction){
    view_mat = mat4_look_at(camera.position, target, up_direction);
}

void set_camera_position(vec3_t pos){
    camera.position = pos;
}

void set_camera_forward(float delta_time){
    camera.forward_velocity = vec3_mul(camera.direction, 5.0*delta_time);
    camera.position = vec3_add(camera.position, camera.forward_velocity);
}

void set_camera_translate(float delta){
    camera.position.y += delta;
}

void set_camera_fovx(float radian){
    fov_x = radian;
}
void set_camera_fovy(float radian){
    fov_y = radian;
}
void set_camera_znear(float z){
    znear = z;
}
void set_camera_zfar(float z){
    zfar = z;
}
void set_camera_aspectx(float a){
    aspect_x = a;
}
void set_camera_aspecty(float a){
    aspect_y = a;
}
void set_proj_mat(mat4_t mat){
    proj_mat = mat;
}
void set_world_mat(mat4_t mat){
    world_mat = mat;
}

////////////////////////////////////////////////////////////////////////////////
// initializers
////////////////////////////////////////////////////////////////////////////////
float init_znear(float z){
    set_camera_znear(z);
    return z;
}
float init_zfar(float z){
    set_camera_zfar(z);
    return z;
}
float init_aspect_x(float a){
    set_camera_aspectx(a);
    return a;
}
float init_aspect_y(float a){
    set_camera_aspecty(a);
    return a;
}
float init_fovx(float r){
    set_camera_fovx(r);
    return r;
}
float init_fovy(float r){
    set_camera_fovy(r);
    return r;
}

////////////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////////////
float get_fov_x(void){
    return fov_x;
}
float get_fov_y(void){
    return fov_y;
}
float get_aspect_x(void){
    return aspect_x;
}
float get_aspect_y(void){
    return aspect_y;
}
float get_znear(void){
    return znear; // not too small, due to properties of NDC coordinates!
}
float get_zfar(void){
    return zfar;
}
mat4_t get_proj_mat(void){
    return proj_mat;
}
mat4_t get_view_mat(void){
    return view_mat;
}
mat4_t get_world_mat(void){
    return world_mat;
}
camera_t get_camera(void){
    return camera;
}
