/*
 * SovereignDroid Input System - Implementation
 * 
 * Phase 5: Native input handling
 */

#include "input.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "SovereignInput"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Looper IDs
#define LOOPER_ID_SENSOR 1

// ============================================================================
// Initialization
// ============================================================================

int input_init(input_manager_t* input, ALooper* looper) {
    if (!input) {
        LOGE("Invalid input manager");
        return -1;
    }
    
    memset(input, 0, sizeof(input_manager_t));
    
    LOGI("Initializing input system");
    
    // Get sensor manager
    input->sensor_manager = ASensorManager_getInstance();
    if (!input->sensor_manager) {
        LOGE("Failed to get sensor manager");
        return -1;
    }
    
    // Create sensor event queue
    input->sensor_queue = ASensorManager_createEventQueue(
        input->sensor_manager,
        looper,
        LOOPER_ID_SENSOR,
        NULL,
        NULL
    );
    
    if (!input->sensor_queue) {
        LOGE("Failed to create sensor event queue");
        return -1;
    }
    
    // Get accelerometer
    input->accelerometer = ASensorManager_getDefaultSensor(
        input->sensor_manager,
        ASENSOR_TYPE_ACCELEROMETER
    );
    
    if (input->accelerometer) {
        LOGI("Accelerometer available: %s", ASensor_getName(input->accelerometer));
    } else {
        LOGW("Accelerometer not available");
    }
    
    // Get gyroscope
    input->gyroscope = ASensorManager_getDefaultSensor(
        input->sensor_manager,
        ASENSOR_TYPE_GYROSCOPE
    );
    
    if (input->gyroscope) {
        LOGI("Gyroscope available: %s", ASensor_getName(input->gyroscope));
    } else {
        LOGW("Gyroscope not available");
    }
    
    input->initialized = 1;
    LOGI("✅ Input system initialized");
    
    return 0;
}

void input_cleanup(input_manager_t* input) {
    if (!input || !input->initialized) {
        return;
    }
    
    LOGI("Cleaning up input system");
    
    // Disable sensors
    input_enable_sensors(input, 0);
    
    // Destroy sensor event queue
    if (input->sensor_queue) {
        ASensorManager_destroyEventQueue(input->sensor_manager, input->sensor_queue);
    }
    
    memset(input, 0, sizeof(input_manager_t));
    LOGI("Input system cleaned up");
}

// ============================================================================
// Event Handling
// ============================================================================

int input_handle_event(input_manager_t* input, AInputEvent* event) {
    if (!input || !event) {
        return 0;
    }
    
    int32_t event_type = AInputEvent_getType(event);
    
    if (event_type == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t action_masked = action & AMOTION_EVENT_ACTION_MASK;
        
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        
        // Call touch callback if registered
        if (input->touch_callback) {
            input->touch_callback(x, y, action_masked);
        }
        
        return 1;
    }
    
    return 0;
}

void input_process_sensors(input_manager_t* input) {
    if (!input || !input->initialized || !input->sensor_queue) {
        return;
    }
    
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(input->sensor_queue, &event, 1) > 0) {
        switch (event.type) {
            case ASENSOR_TYPE_ACCELEROMETER:
                if (input->accel_callback) {
                    input->accel_callback(
                        event.acceleration.x,
                        event.acceleration.y,
                        event.acceleration.z
                    );
                }
                break;
                
            case ASENSOR_TYPE_GYROSCOPE:
                if (input->gyro_callback) {
                    input->gyro_callback(
                        event.data[0],
                        event.data[1],
                        event.data[2]
                    );
                }
                break;
        }
    }
}

// ============================================================================
// Callbacks
// ============================================================================

void input_set_touch_callback(input_manager_t* input, touch_callback_t callback) {
    if (input) {
        input->touch_callback = callback;
    }
}

void input_set_accel_callback(input_manager_t* input, sensor_callback_t callback) {
    if (input) {
        input->accel_callback = callback;
    }
}

void input_set_gyro_callback(input_manager_t* input, sensor_callback_t callback) {
    if (input) {
        input->gyro_callback = callback;
    }
}

// ============================================================================
// Sensor Control
// ============================================================================

void input_enable_sensors(input_manager_t* input, int enable) {
    if (!input || !input->initialized || !input->sensor_queue) {
        return;
    }
    
    if (enable) {
        // Enable accelerometer
        if (input->accelerometer) {
            ASensorEventQueue_enableSensor(input->sensor_queue, input->accelerometer);
            // Set update rate to 60Hz
            ASensorEventQueue_setEventRate(
                input->sensor_queue,
                input->accelerometer,
                1000000 / 60  // microseconds
            );
            LOGI("Accelerometer enabled (60Hz)");
        }
        
        // Enable gyroscope
        if (input->gyroscope) {
            ASensorEventQueue_enableSensor(input->sensor_queue, input->gyroscope);
            ASensorEventQueue_setEventRate(
                input->sensor_queue,
                input->gyroscope,
                1000000 / 60
            );
            LOGI("Gyroscope enabled (60Hz)");
        }
    } else {
        // Disable all sensors
        if (input->accelerometer) {
            ASensorEventQueue_disableSensor(input->sensor_queue, input->accelerometer);
            LOGI("Accelerometer disabled");
        }
        if (input->gyroscope) {
            ASensorEventQueue_disableSensor(input->sensor_queue, input->gyroscope);
            LOGI("Gyroscope disabled");
        }
    }
}
