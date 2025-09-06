#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

typedef struct{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw; //y
} camera_t;

// Proj Matrix Parameter
extern float fov;
extern float aspect;
extern float znear;
extern float zfar;

// Proj, View, World Matrix
extern mat4_t proj_mat;
extern mat4_t view_mat;
extern mat4_t world_mat;
extern camera_t camera;

#endif // CAMERA_H
