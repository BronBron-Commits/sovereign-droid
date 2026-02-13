// Helper: add a box with alpha to vertex buffer (12 triangles, 36 vertices, 7 floats per vertex)
static void add_box_alpha(float* v, int* idx, float cx, float cy, float cz, float width, float height, float depth, float r, float g, float b, float a) {
    float hw = width * 0.5f, hh = height * 0.5f, hd = depth * 0.5f;
    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;
    float z0 = cz - hd, z1 = cz + hd;
    float face[6][3] = {{x0,y0,z1},{x1,y0,z1},{x1,y1,z1},{x0,y0,z1},{x1,y1,z1},{x0,y1,z1}};
    for(int i=0;i<6;i++) { v[*idx]=face[i][0]; v[*idx+1]=face[i][1]; v[*idx+2]=face[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
    float back[6][3] = {{x1,y0,z0},{x0,y0,z0},{x0,y1,z0},{x1,y0,z0},{x0,y1,z0},{x1,y1,z0}};
    for(int i=0;i<6;i++) { v[*idx]=back[i][0]; v[*idx+1]=back[i][1]; v[*idx+2]=back[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
    float left[6][3] = {{x0,y0,z0},{x0,y0,z1},{x0,y1,z1},{x0,y0,z0},{x0,y1,z1},{x0,y1,z0}};
    for(int i=0;i<6;i++) { v[*idx]=left[i][0]; v[*idx+1]=left[i][1]; v[*idx+2]=left[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
    float right[6][3] = {{x1,y0,z1},{x1,y0,z0},{x1,y1,z0},{x1,y0,z1},{x1,y1,z0},{x1,y1,z1}};
    for(int i=0;i<6;i++) { v[*idx]=right[i][0]; v[*idx+1]=right[i][1]; v[*idx+2]=right[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
    float top[6][3] = {{x0,y1,z1},{x1,y1,z1},{x1,y1,z0},{x0,y1,z1},{x1,y1,z0},{x0,y1,z0}};
    for(int i=0;i<6;i++) { v[*idx]=top[i][0]; v[*idx+1]=top[i][1]; v[*idx+2]=top[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
    float bot[6][3] = {{x0,y0,z0},{x1,y0,z0},{x1,y0,z1},{x0,y0,z0},{x1,y0,z1},{x0,y0,z1}};
    for(int i=0;i<6;i++) { v[*idx]=bot[i][0]; v[*idx+1]=bot[i][1]; v[*idx+2]=bot[i][2]; v[*idx+3]=r; v[*idx+4]=g; v[*idx+5]=b; v[*idx+6]=a; *idx+=7; }
}
/*
 * SovereignDroid Native Renderer - Implementation
 * 
 * Phase 5: OpenGL ES 3.0 rendering engine
 * Full native GPU control with EGL context management
 */


#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include "renderer.h"
#include <android/log.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define LOG_TAG "SovereignRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define PI 3.14159265f
#define ISO_YAW_DEG 45.0f
#define ISO_PITCH_DEG 35.264f

// ============================================================================
// Shader Source Code
// ============================================================================

// Simple vertex shader - transforms vertices and passes color
static const char* vertex_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec3 aPosition;\n"
    "layout(location = 1) in vec4 aColor;\n"
    "out vec4 vColor;\n"
    "uniform mat4 uMVP;\n"
    "void main() {\n"
    "    gl_Position = uMVP * vec4(aPosition, 1.0);\n"
    "    vColor = aColor;\n"
    "}\n";

// Simple fragment shader - outputs interpolated color
static const char* fragment_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 vColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vColor;\n"
    "}\n";

// Textured vertex shader
static const char* textured_vertex_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec3 aPosition;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 vTexCoord;\n"
    "uniform mat4 uMVP;\n"
    "void main() {\n"
    "    gl_Position = uMVP * vec4(aPosition, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

// Textured fragment shader
static const char* textured_fragment_shader_source =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 vTexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "void main() {\n"
    "    FragColor = texture(uTexture, vTexCoord);\n"
    "}\n";


typedef struct {
    float m[16];
} mat4_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

typedef struct {
    float* data;
    int vertex_count;
} vertex_buffer_t;

static void mat4_identity(mat4_t* out) {
    memset(out->m, 0, sizeof(out->m));
    out->m[0] = 1.0f;
    out->m[5] = 1.0f;
    out->m[10] = 1.0f;
    out->m[15] = 1.0f;
}

static mat4_t mat4_mul(const mat4_t* a, const mat4_t* b) {
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

static mat4_t mat4_translate(float x, float y, float z) {
    mat4_t out;
    mat4_identity(&out);
    out.m[12] = x;
    out.m[13] = y;
    out.m[14] = z;
    return out;
}

static mat4_t mat4_rotate_x(float angle) {
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

static mat4_t mat4_rotate_y(float angle) {
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


static mat4_t mat4_ortho(float left, float right, float bottom, float top, float near_z, float far_z) {
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

static vec4_t mat4_mul_vec4(const mat4_t* m, vec4_t v) {
    vec4_t out;
    out.x = m->m[0] * v.x + m->m[4] * v.y + m->m[8] * v.z + m->m[12] * v.w;
    out.y = m->m[1] * v.x + m->m[5] * v.y + m->m[9] * v.z + m->m[13] * v.w;
    out.z = m->m[2] * v.x + m->m[6] * v.y + m->m[10] * v.z + m->m[14] * v.w;
    out.w = m->m[3] * v.x + m->m[7] * v.y + m->m[11] * v.z + m->m[15] * v.w;
    return out;
}

static void vec3_normalize(float* x, float* y, float* z) {
    float len = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    if (len > 0.0001f) {
        *x /= len;
        *y /= len;
        *z /= len;
    }
}

static vertex_buffer_t build_grid_vertices(int half_extent, float spacing) {
    int line_count = (half_extent * 2 + 1) * 2;
    int axis_lines = 3;
    int total_lines = line_count + axis_lines;
    int vertex_count = total_lines * 2;
    int float_count = vertex_count * 6;

    float* data = (float*)malloc(sizeof(float) * float_count);
    if (!data) {
        vertex_buffer_t empty = {0};
        return empty;
    }

    int idx = 0;
    float grid_r = 0.25f;
    float grid_g = 0.25f;
    float grid_b = 0.25f;

    for (int i = -half_extent; i <= half_extent; i++) {
        float z = (float)i * spacing;
        float x0 = -half_extent * spacing;
        float x1 = half_extent * spacing;

        data[idx++] = x0; data[idx++] = 0.0f; data[idx++] = z;
        data[idx++] = (i == 0) ? 0.9f : grid_r;
        data[idx++] = (i == 0) ? 0.2f : grid_g;
        data[idx++] = (i == 0) ? 0.2f : grid_b;

        data[idx++] = x1; data[idx++] = 0.0f; data[idx++] = z;
        data[idx++] = (i == 0) ? 0.9f : grid_r;
        data[idx++] = (i == 0) ? 0.2f : grid_g;
        data[idx++] = (i == 0) ? 0.2f : grid_b;

        float x = (float)i * spacing;
        float z0 = -half_extent * spacing;
        float z1 = half_extent * spacing;

        data[idx++] = x; data[idx++] = 0.0f; data[idx++] = z0;
        data[idx++] = (i == 0) ? 0.2f : grid_r;
        data[idx++] = (i == 0) ? 0.2f : grid_g;
        data[idx++] = (i == 0) ? 0.9f : grid_b;

        data[idx++] = x; data[idx++] = 0.0f; data[idx++] = z1;
        data[idx++] = (i == 0) ? 0.2f : grid_r;
        data[idx++] = (i == 0) ? 0.2f : grid_g;
        data[idx++] = (i == 0) ? 0.9f : grid_b;
    }

    // Y axis line
    data[idx++] = 0.0f; data[idx++] = 0.0f; data[idx++] = 0.0f;
    data[idx++] = 0.2f; data[idx++] = 0.9f; data[idx++] = 0.2f;
    data[idx++] = 0.0f; data[idx++] = half_extent * spacing; data[idx++] = 0.0f;
    data[idx++] = 0.2f; data[idx++] = 0.9f; data[idx++] = 0.2f;

    vertex_buffer_t result = { data, vertex_count };
    return result;
}

static void fill_cursor_vertices(float* data, float cx, float cy, float cz, float size) {
    float r = 1.0f;
    float g = 0.9f;
    float b = 0.1f;

    // X axis line
    data[0] = cx - size; data[1] = cy; data[2] = cz; data[3] = r; data[4] = g; data[5] = b;
    data[6] = cx + size; data[7] = cy; data[8] = cz; data[9] = r; data[10] = g; data[11] = b;

    // Y axis line
    data[12] = cx; data[13] = cy - size; data[14] = cz; data[15] = r; data[16] = g; data[17] = b;
    data[18] = cx; data[19] = cy + size; data[20] = cz; data[21] = r; data[22] = g; data[23] = b;

    // Z axis line
    data[24] = cx; data[25] = cy; data[26] = cz - size; data[27] = r; data[28] = g; data[29] = b;
    data[30] = cx; data[31] = cy; data[32] = cz + size; data[33] = r; data[34] = g; data[35] = b;
}

// Helper: add a box to vertex buffer (12 triangles, 36 vertices)
static void add_box(float* v, int* idx, float cx, float cy, float cz, 
                    float width, float height, float depth,
                    float r, float g, float b) {
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;
    
    // 8 corners
    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;
    float z0 = cz - hd, z1 = cz + hd;
    
    // Front face (2 triangles)
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    
    // Back face
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    
    // Left face
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    
    // Right face
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    
    // Top face
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y1; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    
    // Bottom face
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z0; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x1; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
    v[*idx] = x0; v[*idx+1] = y0; v[*idx+2] = z1; v[*idx+3] = r; v[*idx+4] = g; v[*idx+5] = b; *idx += 6;
}

// Build animated knight character with armor and walk cycle
static void fill_character_vertices(float* data, float walk_phase) {
    int idx = 0;
    
    // Character dimensions (scaled 6.0x for better visibility)
    float scale = 6.0f;
    float helmet_size = 0.17f * scale;
    float visor_height = 0.06f * scale;
    float torso_width = 0.22f * scale;
    float torso_height = 0.32f * scale;
    float torso_depth = 0.16f * scale;
    float limb_width = 0.09f * scale;
    float arm_length = 0.24f * scale;
    float leg_length = 0.32f * scale;
    float shoulder_size = 0.14f * scale;
    float boot_height = 0.1f * scale;
    
    // Metallic armor colors - various shades of silver/steel
    float helmet_r = 0.65f, helmet_g = 0.68f, helmet_b = 0.72f;     // Light steel
    float visor_r = 0.15f, visor_g = 0.15f, visor_b = 0.2f;         // Dark visor slit
    float chest_r = 0.55f, chest_g = 0.58f, chest_b = 0.62f;        // Chest plate
    float shoulder_r = 0.5f, shoulder_g = 0.52f, shoulder_b = 0.55f; // Shoulder pauldrons
    float arm_r = 0.45f, arm_g = 0.47f, arm_b = 0.5f;               // Arm armor
    float gauntlet_r = 0.5f, gauntlet_g = 0.52f, gauntlet_b = 0.54f; // Hand armor
    float leg_r = 0.48f, leg_g = 0.5f, leg_b = 0.53f;               // Leg armor
    float boot_r = 0.35f, boot_g = 0.36f, boot_b = 0.38f;           // Darker boots
    
    // Body positions - properly stacked vertically
    float boot_base = boot_height * 0.5f;  // Boots at ground level
    float leg_base = boot_height + leg_length * 0.5f;  // Legs above boots
    float torso_base = boot_height + leg_length + torso_height * 0.5f;  // Torso above legs
    float head_y = boot_height + leg_length + torso_height + helmet_size * 0.5f;  // Head at top
    float torso_y = torso_base;
    float neck_y = boot_height + leg_length + torso_height;
    
    // Walk animation - marching motion with knee bend
    float arm_swing = sinf(walk_phase) * 0.45f;  // Exaggerated forward/back arm swing for march
    float leg_swing = sinf(walk_phase) * 0.18f;  // Forward/back leg movement
    
    // Knee lift for marching - alternate legs lift up
    float left_knee_lift = (sinf(walk_phase) > 0.0f) ? sinf(walk_phase) * leg_length * 0.4f : 0.0f;
    float right_knee_lift = (sinf(walk_phase) < 0.0f) ? -sinf(walk_phase) * leg_length * 0.4f : 0.0f;
    
    // === HELMET ===
    // Main helmet
    add_box(data, &idx, 0.0f, head_y, 0.0f, helmet_size, helmet_size, helmet_size, helmet_r, helmet_g, helmet_b);
    // Visor slit (dark horizontal band)
    add_box(data, &idx, 0.0f, head_y + 0.01f, helmet_size * 0.51f, helmet_size * 0.8f, visor_height, 0.02f, visor_r, visor_g, visor_b);
    // Neck guard
    add_box(data, &idx, 0.0f, neck_y, 0.0f, helmet_size * 0.9f, 0.05f, helmet_size * 0.9f, helmet_r, helmet_g, helmet_b);
    
    // === TORSO ARMOR ===
    // Chest plate (larger than torso)
    add_box(data, &idx, 0.0f, torso_y + 0.05f, 0.02f, torso_width * 1.1f, torso_height * 0.85f, torso_depth * 0.5f, chest_r, chest_g, chest_b);
    // Back plate
    add_box(data, &idx, 0.0f, torso_y + 0.03f, -0.02f, torso_width * 1.05f, torso_height * 0.8f, torso_depth * 0.5f, chest_r, chest_g, chest_b);
    // Waist/belt
    add_box(data, &idx, 0.0f, boot_height + leg_length + torso_height * 0.15f, 0.0f, torso_width * 1.15f, 0.08f * scale, torso_depth * 1.1f, boot_r, boot_g, boot_b);
    
    // === SHOULDER PAULDRONS ===
    float shoulder_y = boot_height + leg_length + torso_height;
    // Left shoulder
    add_box(data, &idx, -torso_width * 0.6f, shoulder_y, 0.0f, shoulder_size, shoulder_size * 0.7f, shoulder_size, shoulder_r, shoulder_g, shoulder_b);
    // Right shoulder
    add_box(data, &idx, torso_width * 0.6f, shoulder_y, 0.0f, shoulder_size, shoulder_size * 0.7f, shoulder_size, shoulder_r, shoulder_g, shoulder_b);
    
    // === ARMS WITH ARMOR ===
    // Left arm (swings opposite to right leg)
    float left_arm_x = -torso_width * 0.6f;
    float left_arm_y = boot_height + leg_length + torso_height * 0.75f - arm_length * 0.5f;
    float left_arm_z = arm_swing;
    // Upper arm armor
    add_box(data, &idx, left_arm_x, left_arm_y + arm_length * 0.25f, left_arm_z, limb_width * 1.1f, arm_length * 0.5f, limb_width * 1.1f, arm_r, arm_g, arm_b);
    // Forearm armor
    add_box(data, &idx, left_arm_x, left_arm_y - arm_length * 0.25f, left_arm_z, limb_width, arm_length * 0.5f, limb_width, arm_r, arm_g, arm_b);
    // Left gauntlet
    add_box(data, &idx, left_arm_x, left_arm_y - arm_length * 0.55f, left_arm_z, limb_width * 1.15f, arm_length * 0.15f, limb_width * 1.15f, gauntlet_r, gauntlet_g, gauntlet_b);
    
    // Right arm (swings opposite to left leg)
    float right_arm_x = torso_width * 0.6f;
    float right_arm_y = boot_height + leg_length + torso_height * 0.75f - arm_length * 0.5f;
    float right_arm_z = -arm_swing;
    // Upper arm armor
    add_box(data, &idx, right_arm_x, right_arm_y + arm_length * 0.25f, right_arm_z, limb_width * 1.1f, arm_length * 0.5f, limb_width * 1.1f, arm_r, arm_g, arm_b);
    // Forearm armor
    add_box(data, &idx, right_arm_x, right_arm_y - arm_length * 0.25f, right_arm_z, limb_width, arm_length * 0.5f, limb_width, arm_r, arm_g, arm_b);
    // Right gauntlet
    add_box(data, &idx, right_arm_x, right_arm_y - arm_length * 0.55f, right_arm_z, limb_width * 1.15f, arm_length * 0.15f, limb_width * 1.15f, gauntlet_r, gauntlet_g, gauntlet_b);
    
    // === LEGS WITH ARMOR ===
    // Left leg (with knee lift for marching)
    float left_leg_x = -torso_width * 0.2f;
    float left_leg_y = leg_base + left_knee_lift;
    float left_leg_z = -leg_swing;
    float left_boot_y = boot_base + left_knee_lift;
    // Thigh armor
    add_box(data, &idx, left_leg_x, left_leg_y + leg_length * 0.25f, left_leg_z, limb_width * 1.15f, leg_length * 0.5f, limb_width * 1.15f, leg_r, leg_g, leg_b);
    // Shin guard
    add_box(data, &idx, left_leg_x, left_leg_y - leg_length * 0.25f, left_leg_z, limb_width * 1.05f, leg_length * 0.5f, limb_width * 1.05f, leg_r, leg_g, leg_b);
    // Left boot
    add_box(data, &idx, left_leg_x, left_boot_y, left_leg_z, limb_width * 1.2f, boot_height, limb_width * 1.3f, boot_r, boot_g, boot_b);
    
    // Right leg (with knee lift for marching)
    float right_leg_x = torso_width * 0.2f;
    float right_leg_y = leg_base + right_knee_lift;
    float right_leg_z = leg_swing;
    float right_boot_y = boot_base + right_knee_lift;
    // Thigh armor
    add_box(data, &idx, right_leg_x, right_leg_y + leg_length * 0.25f, right_leg_z, limb_width * 1.15f, leg_length * 0.5f, limb_width * 1.15f, leg_r, leg_g, leg_b);
    // Shin guard
    add_box(data, &idx, right_leg_x, right_leg_y - leg_length * 0.25f, right_leg_z, limb_width * 1.05f, leg_length * 0.5f, limb_width * 1.05f, leg_r, leg_g, leg_b);
    // Right boot
    add_box(data, &idx, right_leg_x, right_boot_y, right_leg_z, limb_width * 1.2f, boot_height, limb_width * 1.3f, boot_r, boot_g, boot_b);
}

// Build initial character geometry
static vertex_buffer_t build_character_vertices(void) {
    vertex_buffer_t buffer;
    // Knight armor: 20 body parts (helmet, visor, neck, chest, back, belt, 2 shoulders, 
    // 2x(upper arm, forearm, gauntlet), 2x(thigh, shin, boot)) * 36 vertices per box
    buffer.vertex_count = 20 * 36;
    buffer.data = (float*)malloc(sizeof(float) * buffer.vertex_count * 6);
    
    if (!buffer.data) {
        buffer.vertex_count = 0;
        return buffer;
    }
    
    // Fill with initial pose (walk_phase = 0)
    fill_character_vertices(buffer.data, 0.0f);
    
    return buffer;
}

// Build ground plane with texture coordinates
static vertex_buffer_t build_ground_vertices(float size) {
    vertex_buffer_t buffer;
    // Ground plane: 2 triangles = 6 vertices
    // Each vertex: 3 floats (position) + 2 floats (texcoord) = 5 floats
    buffer.vertex_count = 6;
    buffer.data = (float*)malloc(sizeof(float) * buffer.vertex_count * 5);
    
    if (!buffer.data) {
        buffer.vertex_count = 0;
        return buffer;
    }
    
    float half = size;
    float* v = buffer.data;
    
    // Triangle 1
    // Vertex 0: (-half, 0, -half), UV (0, 0)
    v[0] = -half; v[1] = -0.01f; v[2] = -half; v[3] = 0.0f; v[4] = 0.0f;
    // Vertex 1: (half, 0, -half), UV (1, 0)
    v[5] = half; v[6] = -0.01f; v[7] = -half; v[8] = 1.0f; v[9] = 0.0f;
    // Vertex 2: (half, 0, half), UV (1, 1)
    v[10] = half; v[11] = -0.01f; v[12] = half; v[13] = 1.0f; v[14] = 1.0f;
    
    // Triangle 2
    // Vertex 3: (-half, 0, -half), UV (0, 0)
    v[15] = -half; v[16] = -0.01f; v[17] = -half; v[18] = 0.0f; v[19] = 0.0f;
    // Vertex 4: (half, 0, half), UV (1, 1)
    v[20] = half; v[21] = -0.01f; v[22] = half; v[23] = 1.0f; v[24] = 1.0f;
    // Vertex 5: (-half, 0, half), UV (0, 1)
    v[25] = -half; v[26] = -0.01f; v[27] = half; v[28] = 0.0f; v[29] = 1.0f;
    
    return buffer;
}

// Generate procedural checkerboard texture
static GLuint create_checkerboard_texture(int size, int checker_size) {
    int num_pixels = size * size;
    unsigned char* data = (unsigned char*)malloc(num_pixels * 3);
    
    if (!data) {
        LOGE("Failed to allocate texture data");
        return 0;
    }
    
    // Generate checkerboard pattern
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int checker_x = x / checker_size;
            int checker_y = y / checker_size;
            int is_white = (checker_x + checker_y) % 2;
            
            int idx = (y * size + x) * 3;
            if (is_white) {
                data[idx + 0] = 220;  // R
                data[idx + 1] = 220;  // G
                data[idx + 2] = 220;  // B
            } else {
                data[idx + 0] = 100;  // R
                data[idx + 1] = 100;  // G
                data[idx + 2] = 100;  // B
            }
        }
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}

// Generate procedural brushed metal texture
static GLuint create_metal_texture(int size) {
    int num_pixels = size * size;
    unsigned char* data = (unsigned char*)malloc(num_pixels * 3);
    
    if (!data) {
        LOGE("Failed to allocate metal texture data");
        return 0;
    }
    
    // Generate brushed metal pattern with horizontal streaks
    for (int y = 0; y < size; y++) {
        // Base metallic value with vertical variation
        int base_val = 150 + (y % 20) * 3;
        
        for (int x = 0; x < size; x++) {
            int idx = (y * size + x) * 3;
            
            // Add horizontal brush strokes
            int noise = ((x * 7 + y * 11) % 30) - 15;
            int val = base_val + noise;
            
            //  Clamp values
            if (val < 120) val = 120;
            if (val > 200) val = 200;
            
            // Slightly blue-tinted steel color
            data[idx + 0] = val - 10;     // R (slightly less red)
            data[idx + 1] = val - 5;      // G
            data[idx + 2] = val;          // B (slightly more blue)
        }
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    LOGI("Created checkerboard texture: %dx%d", size, size);
    return texture;
}

// ============================================================================
// Shader Compilation
// ============================================================================

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        LOGE("Failed to create shader");
        return 0;
    }
    
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check compilation status
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 0) {
            char* info_log = (char*)malloc(info_len);
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            LOGE("Shader compilation failed: %s", info_log);
            free(info_log);
        }
        glDeleteShader(shader);
        return 0;
    }
    
    LOGI("Shader compiled successfully (type=%d)", type);
    return shader;
}

static GLuint create_program(const char* vertex_src, const char* fragment_src) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
    if (vertex_shader == 0) {
        return 0;
    }
    
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    if (program == 0) {
        LOGE("Failed to create program");
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check link status
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint info_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 0) {
            char* info_log = (char*)malloc(info_len);
            glGetProgramInfoLog(program, info_len, NULL, info_log);
            LOGE("Program linking failed: %s", info_log);
            free(info_log);
        }
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    LOGI("Shader program linked successfully");
    return program;
}

// ============================================================================
// EGL Initialization
// ============================================================================

static int init_egl(renderer_state_t* renderer) {
    LOGI("Initializing EGL");
    
    // Get default display
    renderer->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (renderer->display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return -1;
    }
    
    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(renderer->display, &major, &minor)) {
        LOGE("Failed to initialize EGL");
        return -1;
    }
    LOGI("EGL initialized: version %d.%d", major, minor);
    
    // Choose config
    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    
    EGLint num_configs;
    if (!eglChooseConfig(renderer->display, config_attribs, &renderer->config, 1, &num_configs)) {
        LOGE("Failed to choose EGL config");
        eglTerminate(renderer->display);
        return -1;
    }
    
    if (num_configs == 0) {
        LOGE("No matching EGL configs found");
        eglTerminate(renderer->display);
        return -1;
    }
    
    // Create window surface
    renderer->surface = eglCreateWindowSurface(renderer->display, renderer->config, 
                                               renderer->window, NULL);
    if (renderer->surface == EGL_NO_SURFACE) {
        LOGE("Failed to create EGL surface");
        eglTerminate(renderer->display);
        return -1;
    }
    
    // Create OpenGL ES 3.0 context
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    renderer->context = eglCreateContext(renderer->display, renderer->config, 
                                        EGL_NO_CONTEXT, context_attribs);
    if (renderer->context == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        eglDestroySurface(renderer->display, renderer->surface);
        eglTerminate(renderer->display);
        return -1;
    }
    
    // Make context current
    if (!eglMakeCurrent(renderer->display, renderer->surface, renderer->surface, renderer->context)) {
        LOGE("Failed to make EGL context current");
        eglDestroyContext(renderer->display, renderer->context);
        eglDestroySurface(renderer->display, renderer->surface);
        eglTerminate(renderer->display);
        return -1;
    }
    
    // Query surface dimensions
    eglQuerySurface(renderer->display, renderer->surface, EGL_WIDTH, &renderer->width);
    eglQuerySurface(renderer->display, renderer->surface, EGL_HEIGHT, &renderer->height);
    
    LOGI("EGL context created: %dx%d", renderer->width, renderer->height);
    
    // Log OpenGL info
    LOGI("OpenGL Vendor: %s", glGetString(GL_VENDOR));
    LOGI("OpenGL Renderer: %s", glGetString(GL_RENDERER));
    LOGI("OpenGL Version: %s", glGetString(GL_VERSION));
    LOGI("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    return 0;
}

// ============================================================================
// OpenGL Setup
// ============================================================================

static int init_opengl(renderer_state_t* renderer) {
    LOGI("Initializing OpenGL resources");
    
    // Create shader program
    renderer->shader_program = create_program(vertex_shader_source, fragment_shader_source);
    if (renderer->shader_program == 0) {
        return -1;
    }
    
    // Create textured shader program for ground
    renderer->textured_shader_program = create_program(textured_vertex_shader_source, textured_fragment_shader_source);
    if (renderer->textured_shader_program == 0) {
        return -1;
    }
    
    // Build grid geometry
    vertex_buffer_t grid = build_grid_vertices(renderer->grid_half_extent, renderer->grid_spacing);
    if (!grid.data || grid.vertex_count == 0) {
        LOGE("Failed to allocate grid geometry");
        return -1;
    }

    glGenVertexArrays(1, &renderer->grid_vao);
    glBindVertexArray(renderer->grid_vao);
    glGenBuffers(1, &renderer->grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid.vertex_count * 6, grid.data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    renderer->grid_vertex_count = grid.vertex_count;
    free(grid.data);

    // Cursor geometry
    renderer->cursor_vertex_count = 6;
    glGenVertexArrays(1, &renderer->cursor_vao);
    glBindVertexArray(renderer->cursor_vao);
    glGenBuffers(1, &renderer->cursor_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->cursor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderer->cursor_vertex_count * 6, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Character geometry
    vertex_buffer_t character = build_character_vertices();
    if (!character.data || character.vertex_count == 0) {
        LOGE("Failed to allocate character geometry");
        return -1;
    }
    
    glGenVertexArrays(1, &renderer->character_vao);
    glBindVertexArray(renderer->character_vao);
    glGenBuffers(1, &renderer->character_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->character_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * character.vertex_count * 6, character.data, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    renderer->character_vertex_count = character.vertex_count;
    free(character.data);
    
    // Ground plane geometry with texture coordinates
    float ground_size = (float)renderer->grid_half_extent * renderer->grid_spacing;
    vertex_buffer_t ground = build_ground_vertices(ground_size);
    if (!ground.data || ground.vertex_count == 0) {
        LOGE("Failed to allocate ground geometry");
        return -1;
    }
    glGenVertexArrays(1, &renderer->ground_vao);
    glBindVertexArray(renderer->ground_vao);
    glGenBuffers(1, &renderer->ground_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->ground_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ground.vertex_count * 5, ground.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    renderer->ground_vertex_count = ground.vertex_count;
    free(ground.data);

    // Wall geometry (4 boxes around the floor)
    float wall_thickness = 0.1f * ground_size;
    float wall_height = ground_size * 0.5f;
    float half = ground_size;
    int wall_boxes = 4;
    int box_vertices = 36; // 12 triangles * 3 vertices
    int wall_vertex_count = wall_boxes * box_vertices;
    float* wall_data = (float*)malloc(sizeof(float) * wall_vertex_count * 7);
    int idx = 0;
    float r = 0.6f, g = 0.6f, b = 0.7f, a = 0.4f;
    // +X wall
    add_box_alpha(wall_data, &idx, half, wall_height/2, 0.0f, wall_thickness, wall_height, ground_size*2+wall_thickness, r, g, b, a);
    // -X wall
    add_box_alpha(wall_data, &idx, -half, wall_height/2, 0.0f, wall_thickness, wall_height, ground_size*2+wall_thickness, r, g, b, a);
    // +Z wall
    add_box_alpha(wall_data, &idx, 0.0f, wall_height/2, half, ground_size*2+wall_thickness, wall_height, wall_thickness, r, g, b, a);
    // -Z wall
    add_box_alpha(wall_data, &idx, 0.0f, wall_height/2, -half, ground_size*2+wall_thickness, wall_height, wall_thickness, r, g, b, a);
    glGenVertexArrays(1, &renderer->wall_vao);
    glBindVertexArray(renderer->wall_vao);
    glGenBuffers(1, &renderer->wall_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->wall_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wall_vertex_count * 7, wall_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    renderer->wall_vertex_count = wall_vertex_count;
    free(wall_data);
    
    // Create checkerboard texture (256x256, 32 pixel checkers)
    renderer->ground_texture = create_checkerboard_texture(256, 32);
    if (renderer->ground_texture == 0) {
        LOGE("Failed to create ground texture");
        return -1;
    }
    
    // Set viewport
    glViewport(0, 0, renderer->width, renderer->height);
    
    // Enable depth testing and blending
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepthf(1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    LOGI("OpenGL resources initialized successfully");
    return 0;
}

// ============================================================================
// Public API Implementation
// ============================================================================

int renderer_init(renderer_state_t* renderer, ANativeWindow* window) {
    if (!renderer || !window) {
        LOGE("Invalid parameters");
        return -1;
    }
    
    memset(renderer, 0, sizeof(renderer_state_t));
    renderer->window = window;
    renderer->grid_half_extent = 10;
    renderer->grid_spacing = 1.0f;
    
    LOGI("=== Phase 5: Native 3D Rendering Engine ===");
    LOGI("Renderer build: %s %s", __DATE__, __TIME__);
    
    if (init_egl(renderer) != 0) {
        return -1;
    }

    // Call create_metal_texture to avoid unused function warning
    GLuint metalTex = create_metal_texture(256);
    (void)metalTex;

    if (init_opengl(renderer) != 0) {
        renderer_cleanup(renderer);
        return -1;
    }
    
    renderer->rotation = 0.0f;
    renderer->touch_x = 0.0f;
    renderer->touch_y = 0.0f;
    renderer->cursor_x = 0.0f;
    renderer->cursor_y = 0.0f;
    renderer->cursor_z = 0.0f;
    renderer->character_x = 0.0f;
    renderer->character_y = 0.0f;  // Character boots sit on ground with proper vertical spacing
    renderer->character_z = 0.0f;
    renderer->character_target_x = 0.0f;
    renderer->character_target_z = 0.0f;
    renderer->character_move_speed = 5.0f; // units per second
    renderer->walk_phase = 0.0f;
    renderer->is_moving = 0;
    renderer->facing_direction = 1; // Default facing right
    renderer->facing_angle = 0.0f; // Default facing +X
    renderer->touch_count = 0;
    renderer->touch1_x = 0.0f;
    renderer->touch1_y = 0.0f;
    renderer->touch2_x = 0.0f;
    renderer->touch2_y = 0.0f;
    renderer->prev_pinch_distance = 0.0f;
    renderer->zoom_factor = 1.0f;  // Default zoom
    renderer->background_r = 0.1f;
    renderer->background_g = 0.1f;
    renderer->background_b = 0.2f;
    renderer->initialized = 1;
    renderer->rendering = 1;

    if (renderer->cursor_vbo != 0) {
        float cursor_vertices[36];
        fill_cursor_vertices(cursor_vertices, renderer->cursor_x, renderer->cursor_y, renderer->cursor_z, 0.2f);
        glBindBuffer(GL_ARRAY_BUFFER, renderer->cursor_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cursor_vertices), cursor_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    LOGI("✅ Renderer initialized successfully (OpenGL ES 3.0)");
    return 0;
}

void renderer_cleanup(renderer_state_t* renderer) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    LOGI("Cleaning up renderer resources");
    
    // Clean up OpenGL resources
    if (renderer->grid_vao != 0) {
        glDeleteVertexArrays(1, &renderer->grid_vao);
    }
    if (renderer->grid_vbo != 0) {
        glDeleteBuffers(1, &renderer->grid_vbo);
    }
    if (renderer->cursor_vao != 0) {
        glDeleteVertexArrays(1, &renderer->cursor_vao);
    }
    if (renderer->cursor_vbo != 0) {
        glDeleteBuffers(1, &renderer->cursor_vbo);
    }
    if (renderer->character_vao != 0) {
        glDeleteVertexArrays(1, &renderer->character_vao);
    }
    if (renderer->character_vbo != 0) {
        glDeleteBuffers(1, &renderer->character_vbo);
    }
    if (renderer->ground_vao != 0) {
        glDeleteVertexArrays(1, &renderer->ground_vao);
    }
    if (renderer->ground_vbo != 0) {
        glDeleteBuffers(1, &renderer->ground_vbo);
    }
    if (renderer->ground_texture != 0) {
        glDeleteTextures(1, &renderer->ground_texture);
    }
    if (renderer->shader_program != 0) {
        glDeleteProgram(renderer->shader_program);
    }
    if (renderer->textured_shader_program != 0) {
        glDeleteProgram(renderer->textured_shader_program);
    }
    
    // Clean up EGL
    if (renderer->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(renderer->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (renderer->context != EGL_NO_CONTEXT) {
            eglDestroyContext(renderer->display, renderer->context);
        }
        if (renderer->surface != EGL_NO_SURFACE) {
            eglDestroySurface(renderer->display, renderer->surface);
        }
        eglTerminate(renderer->display);
    }
    
    memset(renderer, 0, sizeof(renderer_state_t));
    LOGI("Renderer cleaned up");
}

void renderer_resize(renderer_state_t* renderer, int32_t width, int32_t height) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    renderer->width = width;
    renderer->height = height;
    glViewport(0, 0, width, height);
    
    LOGI("Renderer resized to: %dx%d", width, height);
}

int renderer_draw_frame(renderer_state_t* renderer) {
    static int logged_once = 0;
    if (!renderer || !renderer->initialized || !renderer->rendering) {
        return -1;
    }
    
    // Clear screen with current background color
    glClearColor(renderer->background_r, renderer->background_g, renderer->background_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(renderer->shader_program);

    // Build MVP matrix
    float aspect = (renderer->height > 0) ? ((float)renderer->width / (float)renderer->height) : 1.0f;
    float ortho_height = (float)renderer->grid_half_extent * renderer->grid_spacing * 1.2f * renderer->zoom_factor;
    float ortho_width = ortho_height * aspect;
    mat4_t proj = mat4_ortho(-ortho_width, ortho_width, -ortho_height, ortho_height, -20.0f, 20.0f);
    mat4_t rot_x = mat4_rotate_x(ISO_PITCH_DEG * PI / 180.0f);
    mat4_t rot_y = mat4_rotate_y(ISO_YAW_DEG * PI / 180.0f);
    mat4_t view = mat4_mul(&rot_y, &rot_x);
    
    // Draw ground plane with textured shader
    glUseProgram(renderer->textured_shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->ground_texture);
    GLint tex_loc = glGetUniformLocation(renderer->textured_shader_program, "uTexture");
    if (tex_loc >= 0) {
        glUniform1i(tex_loc, 0);
    }
    
    mat4_t ground_model = mat4_translate(0.0f, 0.0f, 0.0f);
    mat4_t ground_view = mat4_mul(&view, &ground_model);
    mat4_t ground_mvp = mat4_mul(&proj, &ground_view);
    GLint textured_mvp_loc = glGetUniformLocation(renderer->textured_shader_program, "uMVP");
    if (textured_mvp_loc >= 0) {
        glUniformMatrix4fv(textured_mvp_loc, 1, GL_FALSE, ground_mvp.m);
    }
    
    glBindVertexArray(renderer->ground_vao);
    glDrawArrays(GL_TRIANGLES, 0, renderer->ground_vertex_count);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Draw walls (color shader)
    glUseProgram(renderer->shader_program);
    GLint mvp_loc = glGetUniformLocation(renderer->shader_program, "uMVP");
    if (mvp_loc < 0 && !logged_once) {
        LOGE("uMVP uniform not found");
    }
    mat4_t wall_model = mat4_translate(0.0f, 0.0f, 0.0f);
    mat4_t wall_view = mat4_mul(&view, &wall_model);
    mat4_t wall_mvp = mat4_mul(&proj, &wall_view);
    if (mvp_loc >= 0) {
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, wall_mvp.m);
    }
    glBindVertexArray(renderer->wall_vao);
    glDrawArrays(GL_TRIANGLES, 0, renderer->wall_vertex_count);
    glBindVertexArray(0);

    // Draw cursor marker
    mat4_t cursor_model = mat4_translate(renderer->cursor_x, renderer->cursor_y, renderer->cursor_z);
    mat4_t cursor_view = mat4_mul(&view, &cursor_model);
    mat4_t cursor_mvp = mat4_mul(&proj, &cursor_view);
    if (mvp_loc >= 0) {
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, cursor_mvp.m);
    }

    glLineWidth(2.0f);
    glBindVertexArray(renderer->cursor_vao);
    glDrawArrays(GL_LINES, 0, renderer->cursor_vertex_count);
    glBindVertexArray(0);

    // Draw character rotated to face movement direction
    mat4_t character_model = mat4_translate(renderer->character_x, renderer->character_y, renderer->character_z);
    mat4_t face_rot = mat4_rotate_y(renderer->facing_angle);
    mat4_t character_model_rot = mat4_mul(&character_model, &face_rot);
    mat4_t character_view = mat4_mul(&view, &character_model_rot);
    mat4_t character_mvp = mat4_mul(&proj, &character_view);
    if (mvp_loc >= 0) {
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, character_mvp.m);
    }

    glBindVertexArray(renderer->character_vao);
    glDrawArrays(GL_TRIANGLES, 0, renderer->character_vertex_count);
    glBindVertexArray(0);

    if (!logged_once) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            LOGE("OpenGL error after draw: 0x%04x", err);
        } else {
            LOGI("First frame drawn");
        }
        logged_once = 1;
    }
    
    // Swap buffers
    if (!eglSwapBuffers(renderer->display, renderer->surface)) {
        LOGE("Failed to swap buffers");
        return -1;
    }
    
    return 0;
}

void renderer_update(renderer_state_t* renderer, float delta_time) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    // Rotate triangle (1 revolution per 3 seconds)
    renderer->rotation += delta_time * 2.0f * 3.14159f / 3.0f;
    
    // Keep rotation in [0, 2π]
    if (renderer->rotation > 6.28318f) {
        renderer->rotation -= 6.28318f;
    }

    // Move character toward target
    float dx = renderer->character_target_x - renderer->character_x;
    float dz = renderer->character_target_z - renderer->character_z;
    float dist = sqrtf(dx * dx + dz * dz);
    
    if (dist > 0.005f) { // Lower threshold to trigger animation on small moves
        renderer->is_moving = 1;
        float move_dist = renderer->character_move_speed * delta_time;
        if (move_dist > dist) {
            // Snap to target
            renderer->character_x = renderer->character_target_x;
            renderer->character_z = renderer->character_target_z;
        } else {
            // Move toward target
            renderer->character_x += (dx / dist) * move_dist;
            renderer->character_z += (dz / dist) * move_dist;
        }
        // Update facing direction: left if moving left, right if moving right
        if (fabsf(dx) > 0.001f) {
            renderer->facing_direction = (dx > 0) ? 1 : -1;
        }
        // Update facing angle (atan2 for ground movement, match OpenGL axes)
        if (fabsf(dx) > 0.001f || fabsf(dz) > 0.001f) {
            renderer->facing_angle = atan2f(dx, dz);
        }
        // Update walk animation (complete cycle every 0.5 seconds)
        renderer->walk_phase += delta_time * 3.14159f * 4.0f;
        if (renderer->walk_phase > 6.28318f) {
            renderer->walk_phase -= 6.28318f;
        }
    } else {
        renderer->is_moving = 0;
        // Reset to idle pose
        renderer->walk_phase = 0.0f;
    }
    
    // Update character vertices with current animation
    if (renderer->character_vbo != 0) {
        float* char_vertices = (float*)malloc(sizeof(float) * renderer->character_vertex_count * 6);
        if (char_vertices) {
            fill_character_vertices(char_vertices, renderer->walk_phase);
            glBindBuffer(GL_ARRAY_BUFFER, renderer->character_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * renderer->character_vertex_count * 6, char_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            free(char_vertices);
        }
    }
}

void renderer_handle_touch(renderer_state_t* renderer, float x, float y) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    // Normalize touch coordinates to [-1, 1]
    renderer->touch_x = (x / (float)renderer->width) * 2.0f - 1.0f;
    renderer->touch_y = 1.0f - (y / (float)renderer->height) * 2.0f;

    float aspect = (renderer->height > 0) ? ((float)renderer->width / (float)renderer->height) : 1.0f;
    float ortho_height = (float)renderer->grid_half_extent * renderer->grid_spacing * 1.2f * renderer->zoom_factor;
    float ortho_width = ortho_height * aspect;

    // Screen point in view space (orthographic)
    float vx = renderer->touch_x * ortho_width;
    float vy = renderer->touch_y * ortho_height;
    float vz = 0.0f;

    // Invert the view rotation (inverse of rot_y * rot_x)
    mat4_t inv_rot_x = mat4_rotate_x(-ISO_PITCH_DEG * PI / 180.0f);
    mat4_t inv_rot_y = mat4_rotate_y(-ISO_YAW_DEG * PI / 180.0f);
    mat4_t inv_view = mat4_mul(&inv_rot_x, &inv_rot_y);

    vec4_t origin = mat4_mul_vec4(&inv_view, (vec4_t){ vx, vy, vz, 1.0f });
    vec4_t dir = mat4_mul_vec4(&inv_view, (vec4_t){ 0.0f, 0.0f, -1.0f, 0.0f });

    float dx = dir.x;
    float dy = dir.y;
    float dz = dir.z;
    vec3_normalize(&dx, &dy, &dz);

    float t = 0.0f;
    if (fabsf(dy) > 0.0001f) {
        t = -origin.y / dy;
    }

    float grid_x = origin.x + dx * t;
    float grid_z = origin.z + dz * t;

    float extent = renderer->grid_half_extent * renderer->grid_spacing;
    float snapped_x = roundf(grid_x / renderer->grid_spacing) * renderer->grid_spacing;
    float snapped_z = roundf(grid_z / renderer->grid_spacing) * renderer->grid_spacing;

    if (snapped_x > extent) snapped_x = extent;
    if (snapped_x < -extent) snapped_x = -extent;
    if (snapped_z > extent) snapped_z = extent;
    if (snapped_z < -extent) snapped_z = -extent;

    renderer->cursor_x = snapped_x;
    renderer->cursor_y = 0.0f;
    renderer->cursor_z = snapped_z;

    // Set character target to cursor position
    renderer->character_target_x = snapped_x;
    renderer->character_target_z = snapped_z;

    if (renderer->cursor_vbo != 0) {
        float cursor_vertices[36];
        fill_cursor_vertices(cursor_vertices, renderer->cursor_x, renderer->cursor_y, renderer->cursor_z, 0.2f);
        glBindBuffer(GL_ARRAY_BUFFER, renderer->cursor_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cursor_vertices), cursor_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    LOGI("Cursor grid: (%.1f, %.1f, %.1f)", renderer->cursor_x, renderer->cursor_y, renderer->cursor_z);
}

void renderer_handle_multi_touch(renderer_state_t* renderer, int pointer_count,
                                  float x1, float y1, float x2, float y2) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    renderer->touch_count = pointer_count;
    
    if (pointer_count >= 2) {
        // Two finger pinch zoom
        renderer->touch1_x = x1;
        renderer->touch1_y = y1;
        renderer->touch2_x = x2;
        renderer->touch2_y = y2;
        
        // Calculate distance between two touches
        float dx = x2 - x1;
        float dy = y2 - y1;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (renderer->prev_pinch_distance > 0.0f) {
            // Calculate zoom change
            float distance_change = distance - renderer->prev_pinch_distance;
            float zoom_sensitivity = 0.002f;  // Adjust sensitivity
            
            // Update zoom factor (smaller = zoomed in, larger = zoomed out)
            renderer->zoom_factor -= distance_change * zoom_sensitivity;
            
            // Clamp zoom (0.15 = max zoom in, 5.0 = max zoom out)
            if (renderer->zoom_factor < 0.15f) renderer->zoom_factor = 0.15f;
            if (renderer->zoom_factor > 5.0f) renderer->zoom_factor = 5.0f;
            
            LOGI("Zoom: %.2f (distance: %.1f)", renderer->zoom_factor, distance);
        }
        
        renderer->prev_pinch_distance = distance;
    } else if (pointer_count == 1) {
        // Single touch - handle as cursor movement
        renderer_handle_touch(renderer, x1, y1);
        renderer->prev_pinch_distance = 0.0f;  // Reset pinch tracking
    } else {
        // No touches
        renderer->prev_pinch_distance = 0.0f;
    }
}

void renderer_set_background(renderer_state_t* renderer, float r, float g, float b) {
    if (!renderer || !renderer->initialized) {
        return;
    }

    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    if (g < 0.0f) g = 0.0f;
    if (g > 1.0f) g = 1.0f;
    if (b < 0.0f) b = 0.0f;
    if (b > 1.0f) b = 1.0f;

    renderer->background_r = r;
    renderer->background_g = g;
    renderer->background_b = b;
}

void renderer_suspend(renderer_state_t* renderer) {
    if (!renderer || !renderer->initialized) {
        return;
    }
    
    renderer->rendering = 0;
    LOGI("Renderer suspended");
}

int renderer_resume(renderer_state_t* renderer, ANativeWindow* window) {
    if (!renderer) {
        return -1;
    }
    
    if (!renderer->initialized) {
        // First time init
        return renderer_init(renderer, window);
    }
    
    // TODO: Recreate EGL surface if window changed
    renderer->rendering = 1;
    LOGI("Renderer resumed");
    return 0;
}
