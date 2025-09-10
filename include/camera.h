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

// Setters
void set_camera_yaw(float delta);
void set_camera_translate(float delta);
void set_camera_forward(float delta_time);
void set_camera_backward(float delta_time);

// Proj Matrix Parameter
extern float fov_x;
extern float fov_y;
extern float aspect_x;
extern float aspect_y;
extern float znear;
extern float zfar;

// Proj, View, World Matrix
extern mat4_t proj_mat;
extern mat4_t view_mat;
extern mat4_t world_mat;
extern camera_t camera;

#endif // CAMERA_H
