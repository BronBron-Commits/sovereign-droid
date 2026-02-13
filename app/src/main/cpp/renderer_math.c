#include "renderer_math.h"
#include <string.h>
#include <math.h>

void mat4_identity(mat4_t* out) {
    memset(out->m, 0, sizeof(out->m));
    out->m[0] = 1.0f;
    out->m[5] = 1.0f;
    out->m[10] = 1.0f;
    out->m[15] = 1.0f;
}

mat4_t mat4_mul(const mat4_t* a, const mat4_t* b) {
    mat4_t out;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            out.m[col * 4 + row] =
                a->m[0 * 4 + row] * b->m[col * 4 + 0] +
                a->m[1 * 4 + row] * b->m[col * 4 + 1] +
                a->m[2 * 4 + row] * b->m[col * 4 + 2] +
                a->m[3 * 4 + row] * b->m[col * 4 + 3];
        }
    }
    return out;
}

mat4_t mat4_translate(float x, float y, float z) {
    mat4_t out;
    mat4_identity(&out);
    out.m[12] = x;
    out.m[13] = y;
    out.m[14] = z;
    return out;
}

mat4_t mat4_rotate_x(float angle) {
    mat4_t out;
    mat4_identity(&out);
    float c = cosf(angle);
    float s = sinf(angle);
    out.m[5] = c;
    out.m[6] = s;
    out.m[9] = -s;
    out.m[10] = c;
    return out;
}

mat4_t mat4_rotate_y(float angle) {
    mat4_t out;
    mat4_identity(&out);
    float c = cosf(angle);
    float s = sinf(angle);
    out.m[0] = c;
    out.m[2] = -s;
    out.m[8] = s;
    out.m[10] = c;
    return out;
}

mat4_t mat4_ortho(float left, float right, float bottom, float top, float near_z, float far_z) {
    mat4_t out;
    memset(out.m, 0, sizeof(out.m));
    out.m[0] = 2.0f / (right - left);
    out.m[5] = 2.0f / (top - bottom);
    out.m[10] = -2.0f / (far_z - near_z);
    out.m[12] = -(right + left) / (right - left);
    out.m[13] = -(top + bottom) / (top - bottom);
    out.m[14] = -(far_z + near_z) / (far_z - near_z);
    out.m[15] = 1.0f;
    return out;
}

vec4_t mat4_mul_vec4(const mat4_t* m, vec4_t v) {
    vec4_t out;
    out.x = m->m[0] * v.x + m->m[4] * v.y + m->m[8] * v.z + m->m[12] * v.w;
    out.y = m->m[1] * v.x + m->m[5] * v.y + m->m[9] * v.z + m->m[13] * v.w;
    out.z = m->m[2] * v.x + m->m[6] * v.y + m->m[10] * v.z + m->m[14] * v.w;
    out.w = m->m[3] * v.x + m->m[7] * v.y + m->m[11] * v.z + m->m[15] * v.w;
    return out;
}

void vec3_normalize(float* x, float* y, float* z) {
    float len = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    if (len > 0.0001f) {
        *x /= len;
        *y /= len;
        *z /= len;
    }
}
