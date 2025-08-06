#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "mesh.h"
#include "array.h"

// global variable: declared in mesh.h, initialized here in mesh.c
mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },
    // right
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },
    // back
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },
    // left
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },
    // top
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },
    // bottom
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void){

    for (int i = 0; i < N_CUBE_VERTICES; i++){
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++){
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

bool load_obj_file_data(char * filename){
    // TODO : Read the contents of .obj file.
    // andn load the vertices and faces in
    // our mesh.vertices and mesh.faces

    FILE *file = fopen(filename, "r");
    if (!file){
        perror("Failed to open .obj file");
        return false;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)){
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            int matched = sscanf(line+2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            if (matched == 3){
                array_push(mesh.vertices, vertex);
            }else{
                return false;
            }
        } else if (strncmp(line, "vt ", 3) == 0){
            // TODO: implement what's for vt!
        } else if (strncmp(line, "vn ", 3) == 0){
            // TODO: implement what's for vn!

        } else if (strncmp(line, "f ", 2) == 0){
            face_t v;
            face_t vt;
            face_t vn;
            int matched = sscanf(line + 2,
                                 "%d/%d/%d %d/%d/%d %d/%d/%d",
                                 &v.a, &vt.a, &vn.a,
                                 &v.b, &vt.b, &vn.b,
                                 &v.c, &vt.c, &vn.c);
            if (matched == 9){
                array_push(mesh.faces, v);
            }else{
                return false;
            }
            // TODO: implement what's for vt and vn.
        }else{
            continue;
        }
    }
    return true;
}
