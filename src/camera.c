#include "camera.h"
#include "matrix.h"
#include "vector.h"
#include "display.h"

// Projection matrix parameters
float fov = 1.04716666667; // PI/3 = 60 degrees
float aspect = 600.0 / 800.0;
float znear = 1.0; // not too small, due to properties of NDC coordinates!
float zfar = 100.0;

// other global variables: camera pos, proj_mat
vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
mat4_t proj_mat;
