#include "matrix.h"

/**
 * @brief returns a 4 x 4 identity matrix
 *
 * @param
 * @return
 */
mat4_t mat4_identity(void){
    mat4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return m;
}

/**
 * @brief returns a 4 x 4 scale matrix
 *
 * @param scale parameters for x, y, z
 * @return
 */
mat4_t mat4_make_scale(float sx, float sy, float sz){
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[0][0] = sx;
    m.m[0][0] = sx;
    return m;
}

/**
 * @brief returns mat4_t * vec4_t.
 *
 * @param a 4x4 matrix and 4D vector
 * @return a 4D vector
 */
vec4_t mat4_mul_vec(mat4_t m, vec4_t v){
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]* v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]* v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]* v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]* v.w;
    return result;
}
