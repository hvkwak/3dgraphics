#include "camera.h"
#include "matrix.h"

// Projection matrix parameters
float fov = 1.04716666667; // PI/3 = 60 degrees
float aspect = 600.0 / 800.0;
float znear = 1.0; // not too small, due to properties of NDC coordinates!
float zfar = 100.0;

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
