#ifndef CLIP_H
#define CLIP_H

#include "vector.h"
#include "triangle.h"
#define MAX_NUM_POLYGON_VERTICES 10
#define MAX_NUM_POLYGON_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct {
    vec3_t vertices[MAX_NUM_POLYGON_VERTICES];
    tex2_t texcoords[MAX_NUM_POLYGON_VERTICES];
    int num_vertices;
} polygon_t;

float float_lerp(float a, float b, float t);
void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar);
void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_traingles);
polygon_t polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2);

void clip_polygon_against_plane(polygon_t* polygon, int plane);
void clip_polygon(polygon_t* polygon);


#endif // CLIP_H
