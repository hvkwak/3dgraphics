#include "util.h"
#include "vector.h"


/**
 * @brief returns barycentric weights in vec3_t
 *
 * @param a, b, c: vertices of triangle
 *        p      : point to render
 * @return
 */
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    // Find the vectors between the vertices ABC and point p
    vec2_t ac = vec2_sub(c, a); // from a to c
    vec2_t ab = vec2_sub(b, a); // from a to b
    vec2_t ap = vec2_sub(p, a); // from a to p
    vec2_t pc = vec2_sub(c, p); // from p to c
    vec2_t pb = vec2_sub(b, p); // from p to b

    // Compute the area of the full parallelogram of ABC using 2D cross product
    float area_full = (ac.x * ab.y - ac.y * ab.x); // AC x AB

    // Alpha is the area of the small parallelogram divided by the area of the full parallelogram
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_full; // ||PC x PB||

    // clamp
    if (alpha > 1.0f) alpha = 0.99999;
    if (alpha < 0.0f) alpha = 0.00001;

    // Beta is the area of the small parallelogram APC divided by the area of the full parallelogram/triangle ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_full; // ||AC x AP||
    if (beta > 1.0) beta = 0.99999;
    if (beta < 0.0) beta = 0.00001;

    // Weight gamma is easily found since barycentric coordinates always add up to 1.0
    float gamma = 1.0 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}
