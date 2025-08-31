#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "mesh.h"
#include "array.h"

// global variable: declared in mesh.h, initialized here in mesh.c
mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
    .translation = {0, 0, 0}
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
    // faces have now uv coordinates for texture mapping.
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
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

/**
 * @brief loads the vertices and faces in mesh.vertices and mesh.faces
 *
 * @param filename
 * @return returns true, when load is successful.
 */
bool load_obj_file_data(char * filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        perror("Failed to open .obj file");
        return false;
    }

    tex2_t* texcoords = NULL;

    char line[512]; // 512 characters per line expected. char: 1 byte
    while (fgets(line, sizeof(line), file)){
        printf("Line = %s", line);
        if (strncmp(line, "v ", 2) == 0) {
            // vertex
            vec3_t vertex;
            int matched = sscanf(line+2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            // OR: sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            if (matched == 3){
                array_push(mesh.vertices, vertex);
            }else{
                return false;
            }
        } else if (strncmp(line, "vt ", 3) == 0){
            // texture coordinate information
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        } else if (strncmp(line, "vn ", 3) == 0){
            // vertex normal
            // TODO: implement what's for vn
        } else if (strncmp(line, "f ", 2) == 0){
            // face
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            int matched = sscanf(line + 2,
                                 "%d/%d/%d %d/%d/%d %d/%d/%d",
                                 &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                                 &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                                 &vertex_indices[2], &texture_indices[2], &normal_indices[2]);
            if (matched == 9){
                face_t face = {
                    .a = vertex_indices[0]-1,
                    .b = vertex_indices[1]-1,
                    .c = vertex_indices[2]-1,
                    .a_uv = texcoords[texture_indices[0]-1],
                    .b_uv = texcoords[texture_indices[1]-1],
                    .c_uv = texcoords[texture_indices[2]-1],
                    .color = 0xFFFFFFFF
                };
                array_push(mesh.faces, face);
            }else{
                return false;
            }
            // TODO: implement what's for vt and vn.
        }else{
            continue;
        }
    }
    array_free(texcoords);
    return true;
}
