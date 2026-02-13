#ifndef RENDERER_GEOMETRY_H
#define RENDERER_GEOMETRY_H

#include <stdint.h>

typedef struct {
    float* data;
    int vertex_count;
} vertex_buffer_t;

void add_box(float* v, int* idx, float cx, float cy, float cz, float width, float height, float depth, float r, float g, float b);
void add_box_alpha(float* v, int* idx, float cx, float cy, float cz, float width, float height, float depth, float r, float g, float b, float a);
vertex_buffer_t build_grid_vertices(int half_extent, float spacing);
void fill_cursor_vertices(float* data, float cx, float cy, float cz, float size);
void fill_character_vertices(float* data, float walk_phase);
vertex_buffer_t build_character_vertices(void);
vertex_buffer_t build_ground_vertices(float size);

#endif // RENDERER_GEOMETRY_H
