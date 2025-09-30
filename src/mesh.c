#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "mesh.h"
#include "array.h"
#include "upng.h"

// static array to handle multiple meshes
#define MAX_NUM_MESHES 10
static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;


mesh_t* get_mesh(int index){
    return &meshes[index];
}

int get_num_meshes(void){
    return mesh_count;
}

bool load_mesh(char* obj_filename,
               char* png_filename,
               vec3_t scale,
               vec3_t translation,
               vec3_t rotation){

    if (!load_mesh_obj_data(&meshes[mesh_count], obj_filename)){
        return false;
    }
    if (!load_mesh_png_data(&meshes[mesh_count], png_filename)){
        return false;
    }
    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translation = translation;
    meshes[mesh_count].rotation = rotation;
    mesh_count++;
    return true;
}

bool load_mesh_png_data(mesh_t *mesh, char* png_filename){
    upng_t* png_image = upng_new_from_file(png_filename);
    if (png_image != NULL){
        upng_decode(png_image);
        if(upng_get_error(png_image) == UPNG_EOK){
            // no error
            mesh->texture = png_image;
            return true;
        }
    }
    return false;
}

bool load_mesh_obj_data(mesh_t* mesh, char * filename){
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
                array_push(mesh->vertices, vertex);
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
                array_push(mesh->faces, face);
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

/**
 * @brief loads the vertices and faces in mesh.vertices and mesh.faces
 *
 * @param filename
 * @return returns true, when load is successful.
 */
/* bool load_obj_file_data(char * filename){ */
/*     FILE *file = fopen(filename, "r"); */
/*     if (!file){ */
/*         perror("Failed to open .obj file"); */
/*         return false; */
/*     } */

/*     tex2_t* texcoords = NULL; */

/*     char line[512]; // 512 characters per line expected. char: 1 byte */
/*     while (fgets(line, sizeof(line), file)){ */
/*         printf("Line = %s", line); */
/*         if (strncmp(line, "v ", 2) == 0) { */
/*             // vertex */
/*             vec3_t vertex; */
/*             int matched = sscanf(line+2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z); */
/*             // OR: sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z); */
/*             if (matched == 3){ */
/*                 array_push(mesh.vertices, vertex); */
/*             }else{ */
/*                 return false; */
/*             } */
/*         } else if (strncmp(line, "vt ", 3) == 0){ */
/*             // texture coordinate information */
/*             tex2_t texcoord; */
/*             sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v); */
/*             array_push(texcoords, texcoord); */
/*         } else if (strncmp(line, "vn ", 3) == 0){ */
/*             // vertex normal */
/*             // TODO: implement what's for vn */
/*         } else if (strncmp(line, "f ", 2) == 0){ */
/*             // face */
/*             int vertex_indices[3]; */
/*             int texture_indices[3]; */
/*             int normal_indices[3]; */
/*             int matched = sscanf(line + 2, */
/*                                  "%d/%d/%d %d/%d/%d %d/%d/%d", */
/*                                  &vertex_indices[0], &texture_indices[0], &normal_indices[0], */
/*                                  &vertex_indices[1], &texture_indices[1], &normal_indices[1], */
/*                                  &vertex_indices[2], &texture_indices[2], &normal_indices[2]); */
/*             if (matched == 9){ */
/*                 face_t face = { */
/*                     .a = vertex_indices[0]-1, */
/*                     .b = vertex_indices[1]-1, */
/*                     .c = vertex_indices[2]-1, */
/*                     .a_uv = texcoords[texture_indices[0]-1], */
/*                     .b_uv = texcoords[texture_indices[1]-1], */
/*                     .c_uv = texcoords[texture_indices[2]-1], */
/*                     .color = 0xFFFFFFFF */
/*                 }; */
/*                 array_push(mesh.faces, face); */
/*             }else{ */
/*                 return false; */
/*             } */
/*             // TODO: implement what's for vt and vn. */
/*         }else{ */
/*             continue; */
/*         } */
/*     } */
/*     array_free(texcoords); */
/*     return true; */
/* } */


void free_mesh(void){
    for (int i = 0; i < mesh_count; i++){
        upng_free(meshes[i].texture);
        if (array_length(meshes[i].vertices) != 0){
            array_free(meshes[i].vertices);
        }
        if (array_length(meshes[i].faces) != 0){
            array_free(meshes[i].faces);
        }
    }
}
