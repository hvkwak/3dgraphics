#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

extern float fov;
extern float aspect;
extern float znear;
extern float zfar;

extern vec3_t camera_position;
extern mat4_t proj_mat;

#endif // CAMERA_H
