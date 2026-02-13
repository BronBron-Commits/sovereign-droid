/*
 * SovereignDroid Input System
 * 
 * Phase 5: Native input handling
 * Touch and sensor input processing with zero Java overhead
 */

#ifndef SOVEREIGNDROID_INPUT_H
#define SOVEREIGNDROID_INPUT_H

#include <android/input.h>
#include <android/sensor.h>
#include <android/looper.h>

#ifdef __cplusplus
extern "C" {
#endif

// Input callbacks
typedef void (*touch_callback_t)(float x, float y, int32_t action);
typedef void (*sensor_callback_t)(float x, float y, float z);

// Input manager state
typedef struct {
    // Callbacks
    touch_callback_t touch_callback;
    sensor_callback_t accel_callback;
    sensor_callback_t gyro_callback;
    
    // Sensor state
    ASensorManager* sensor_manager;
    ASensorEventQueue* sensor_queue;
    const ASensor* accelerometer;
    const ASensor* gyroscope;
    
    int initialized;
} input_manager_t;

/*
 * Initialize input system
 * Returns 0 on success, -1 on failure
 */
int input_init(input_manager_t* input, ALooper* looper);

/*
 * Cleanup input resources
 */
void input_cleanup(input_manager_t* input);

/*
 * Process input event
 * Returns 1 if handled, 0 if not handled
 */
int input_handle_event(input_manager_t* input, AInputEvent* event);

/*
 * Process sensor events
 * Called from looper callback
 */
void input_process_sensors(input_manager_t* input);

/*
 * Set touch callback
 */
void input_set_touch_callback(input_manager_t* input, touch_callback_t callback);

/*
 * Set accelerometer callback
 */
void input_set_accel_callback(input_manager_t* input, sensor_callback_t callback);

/*
 * Set gyroscope callback
 */
void input_set_gyro_callback(input_manager_t* input, sensor_callback_t callback);

/*
 * Enable/disable sensors
 */
void input_enable_sensors(input_manager_t* input, int enable);

#ifdef __cplusplus
}
#endif

#endif // SOVEREIGNDROID_INPUT_H
