#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include <stdbool.h>
#include "upng.h"

// Define a struct for dynamic size meshes, with array of
// vertices and faces.
typedef struct{
    vec3_t* vertices;   // mesh dynamic array of vertices
    face_t* faces;      // mesh dynamic array of faces
    upng_t* texture;    // mesh PNG texture pointer
    vec3_t rotation;    // mesh rotation with x, y, and z values
    vec3_t scale;       // mesh scale with x, y, and z values
    vec3_t translation; // mesh translation with x, y, and z values
} mesh_t;

bool load_mesh(char* obj_filename,
               char* png_filename,
               vec3_t scale,
               vec3_t translation,
               vec3_t rotation);
/* bool load_obj_file_data(char * filename); */
bool load_mesh_obj_data(mesh_t* mesh, char * filename);
bool load_mesh_png_data(mesh_t* mesh, char * filename);
mesh_t* get_mesh(int index);
void free_mesh(void);
int get_num_meshes(void);

#endif // MESH_H
