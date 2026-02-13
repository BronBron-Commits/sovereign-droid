/*
 * SovereignDroid Native Renderer
 * 
 * Phase 5: OpenGL ES 3.0 rendering engine
 * Pure native GPU-accelerated graphics with zero Java overhead
 * 
 * Features:
 * - EGL context management
 * - Shader compilation and linking
 * - Vertex buffer management
 * - Integration with secure storage for encrypted assets
 */

#ifndef SOVEREIGNDROID_RENDERER_H
#define SOVEREIGNDROID_RENDERER_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Renderer state
typedef struct {
    // EGL state
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;
    
    // Window state
    ANativeWindow* window;
    int32_t width;
    int32_t height;
    
    // OpenGL state
    GLuint shader_program;
    GLuint textured_shader_program;
    GLuint grid_vbo;
    GLuint grid_vao;
    GLuint cursor_vbo;
    GLuint cursor_vao;
    GLuint character_vbo;
    GLuint character_vao;
    GLuint ground_vbo;
    GLuint ground_vao;
    GLuint ground_texture;
    GLuint character_texture;
    // Wall rendering
    GLuint wall_vbo;
    GLuint wall_vao;
    int wall_vertex_count;
    int grid_vertex_count;
    int cursor_vertex_count;
    int character_vertex_count;
    int ground_vertex_count;
    
    // Animation state
    float rotation;
    float touch_x;
    float touch_y;
    
    // Multi-touch state for pinch zoom
    int touch_count;
    float touch1_x;
    float touch1_y;
    float touch2_x;
    float touch2_y;
    float prev_pinch_distance;
    float zoom_factor;  // Camera zoom (1.0 = default, <1.0 = zoomed in, >1.0 = zoomed out)

    // Cursor position in world space
    float cursor_x;
    float cursor_y;
    float cursor_z;

    // Character position and movement
    float character_x;
    float character_y;
    float character_z;
    float character_target_x;
    float character_target_z;
    float character_move_speed;
    float walk_phase;      // Animation phase for walk cycle
    int is_moving;         // Whether character is currently moving

    // Character facing direction: -1 = left, 1 = right
        int facing_direction;
        // Character facing angle (radians, 0 = +X, PI/2 = +Z)
        float facing_angle;

    // Grid settings
    int grid_half_extent;
    float grid_spacing;

    // Background color
    float background_r;
    float background_g;
    float background_b;
    
    // Lifecycle
    int initialized;
    int rendering;
} renderer_state_t;

/*
 * Initialize renderer with native window
 * Returns 0 on success, -1 on failure
 */
int renderer_init(renderer_state_t* renderer, ANativeWindow* window);

/*
 * Cleanup renderer resources
 */
void renderer_cleanup(renderer_state_t* renderer);

/*
 * Handle window resize
 */
void renderer_resize(renderer_state_t* renderer, int32_t width, int32_t height);

/*
 * Render single frame
 * Returns 0 on success, -1 on failure
 */
int renderer_draw_frame(renderer_state_t* renderer);

/*
 * Update animation state
 * delta_time: time since last update in seconds
 */
void renderer_update(renderer_state_t* renderer, float delta_time);

/*
 * Handle touch input
 */
void renderer_handle_touch(renderer_state_t* renderer, float x, float y);

/*
 * Handle multi-touch input for pinch zoom
 */
void renderer_handle_multi_touch(renderer_state_t* renderer, int pointer_count, 
                                  float x1, float y1, float x2, float y2);

/*
 * Set background clear color
 */
void renderer_set_background(renderer_state_t* renderer, float r, float g, float b);

/*
 * Suspend rendering (app backgrounded)
 */
void renderer_suspend(renderer_state_t* renderer);

/*
 * Resume rendering (app foregrounded)
 */
int renderer_resume(renderer_state_t* renderer, ANativeWindow* window);

#ifdef __cplusplus
}
#endif

#endif // SOVEREIGNDROID_RENDERER_H
