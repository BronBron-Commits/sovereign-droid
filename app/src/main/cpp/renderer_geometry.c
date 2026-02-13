#include "renderer_geometry.h"
#include <stdlib.h>

void add_box(float* v, int* idx, float cx, float cy, float cz, float width, float height, float depth, float r, float g, float b) {
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;
    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;
    float z0 = cz - hd, z1 = cz + hd;
    // ...existing code for faces...
}

void add_box_alpha(float* v, int* idx, float cx, float cy, float cz, float width, float height, float depth, float r, float g, float b, float a) {
    float hw = width * 0.5f, hh = height * 0.5f, hd = depth * 0.5f;
    float x0 = cx - hw, x1 = cx + hw;
    float y0 = cy - hh, y1 = cy + hh;
    float z0 = cz - hd, z1 = cz + hd;
    // ...existing code for faces with alpha...
}

vertex_buffer_t build_grid_vertices(int half_extent, float spacing) {
    // ...existing code...
}

void fill_cursor_vertices(float* data, float cx, float cy, float cz, float size) {
    // ...existing code...
}

void fill_character_vertices(float* data, float walk_phase) {
    // ...existing code...
}

vertex_buffer_t build_character_vertices(void) {
    // ...existing code...
}

vertex_buffer_t build_ground_vertices(float size) {
    // ...existing code...
}
