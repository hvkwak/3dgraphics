#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

typedef struct{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw; //y
    float pitch;
} camera_t;

// Setters
void rotate_camera_yaw(float angle);
void rotate_camera_pitch(float angle);
void set_camera_direction(vec3_t target);
void set_camera_position(vec3_t pos);
void set_camera_translate_x(float delta);
void set_camera_translate_y(float delta);
void set_camera_forward(float delta_time);
void set_camera_fovx(float radian);
void set_camera_fovy(float radian);
void set_camera_znear(float z);
void set_camera_zfar(float z);
void set_camera_aspectx(float a);
void set_camera_aspecty(float a);
void set_proj_mat(mat4_t mat);
void set_view_mat(vec3_t target, vec3_t up_direction);
void set_target(vec3_t* target);
void set_world_mat(mat4_t mat);

// Intializer
void init_camera(vec3_t position, vec3_t direction);
float init_znear(float z);
float init_zfar(float z);
float init_aspect_x(float a);
float init_aspect_y(float a);
float init_fovx(float r);
float init_fovy(float r);

// Getters
float get_fov_x(void); // PI/3 = 60 degrees
float get_fov_y(void); // PI/3 = 60 degrees
float get_aspect_x(void); // PI/3 = 60 degrees
float get_aspect_y(void);
float get_znear(void); // not too small, due to properties of NDC coordinates!
float get_zfar(void);
mat4_t get_proj_mat(void); // projection matrix
mat4_t get_view_mat(void); // look_at matrix
mat4_t get_world_mat(void);
camera_t get_camera(void);
float get_camera_yaw(void);
float get_camera_pitch(void);


#endif // CAMERA_H
