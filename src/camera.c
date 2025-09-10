#include "camera.h"
#include "matrix.h"

// Projection matrix parameters
float fov_x; // PI/3 = 60 degrees
float fov_y; // PI/3 = 60 degrees
float aspect_x; // PI/3 = 60 degrees
float aspect_y;
float znear; // not too small, due to properties of NDC coordinates!
float zfar;

// other global variables: camera pos, proj_mat, look_mat, ...
mat4_t proj_mat; // projection matrix
mat4_t view_mat; // look_at matrix
mat4_t world_mat;

camera_t camera = {
    .position = {0, 0, 0},
    .direction = {0, 0, 1}, // looking at unit position in z, forward.
    .forward_velocity = {0, 0, 0},
    .yaw = 0.0
};


void set_camera_yaw(float delta){
    camera.yaw += delta;
}

void set_camera_forward(float delta_time){
    camera.forward_velocity = vec3_mul(camera.direction, 5.0*delta_time);
    camera.position = vec3_add(camera.position, camera.forward_velocity);
}

void set_camera_backward(float delta_time){
    camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
    camera.position = vec3_sub(camera.position, camera.forward_velocity);
}

void set_camera_translate(float delta){
    camera.position.y += delta;
}
