#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float u;
    float v;
} tex2_t;

bool load_png_texture_data(char* filename);
tex2_t tex2_clone(tex2_t* t);

uint32_t* get_texture(void);
int get_texture_width(void);
int get_texture_height(void);
void free_texture(void);

#endif // TEXTURE_H
