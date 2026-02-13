#ifndef RENDERER_MATH_H
#define RENDERER_MATH_H

#include <stdint.h>

typedef struct {
    float m[16];
} mat4_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

void mat4_identity(mat4_t* out);
mat4_t mat4_mul(const mat4_t* a, const mat4_t* b);
mat4_t mat4_translate(float x, float y, float z);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t mat4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
vec4_t mat4_mul_vec4(const mat4_t* m, vec4_t v);
void vec3_normalize(float* x, float* y, float* z);

#endif // RENDERER_MATH_H
