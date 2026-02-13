/*
 * SovereignDroid Device Capabilities - Implementation
 * 
 * Phase 2: Native-layer device capability enumeration
 * 
 * Purpose:
 * - Query CPU architecture and capabilities
 * - Enumerate available sensors
 * - Check security status
 * - Retrieve build information
 * - Generate comprehensive device fingerprint
 * 
 * This is infrastructure validation, not feature code.
 */

#include "device_capabilities.h"
#include <android/log.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define LOG_TAG "DeviceCapabilities"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Buffer size for system properties
#define PROP_VALUE_MAX 92

/*
 * ========================================================================
 * Native C API for internal use (non-JNI)
 * ========================================================================
 */

/*
 * Initialize device capabilities subsystem
 */
void device_capabilities_init(void) {
    LOGI("Device capabilities subsystem initialized");
}

/*
 * Get CPU architecture (native C API)
 */
void device_capabilities_get_cpu_arch(char* buffer, size_t buffer_size) {
    int len = __system_property_get("ro.product.cpu.abi", buffer);
    if (len <= 0) {
        strncpy(buffer, "unknown", buffer_size);
    }
}

/*
 * Get CPU core count (native C API)
 */
int device_capabilities_get_cpu_cores(void) {
    long cores = sysconf(_SC_NPROCESSORS_CONF);
    return (cores > 0) ? (int)cores : 1;
}

/*
 * Get sensor list (native C API)
 */
void device_capabilities_get_sensors(char* buffer, size_t buffer_size) {
    strncpy(buffer, "accelerometer,gyroscope,magnetometer,proximity,light", buffer_size);
}

/*
 * Get security status (native C API)
 */
void device_capabilities_get_security(char* buffer, size_t buffer_size) {
    char selinux[PROP_VALUE_MAX];
    char debuggable[PROP_VALUE_MAX];
    char encrypted[PROP_VALUE_MAX];
    
    __system_property_get("ro.boot.selinux", selinux);
    if (strlen(selinux) == 0) {
        strcpy(selinux, "unknown");
    }
    
    int prop_result = __system_property_get("ro.debuggable", debuggable);
    if (prop_result <= 0) {
        strcpy(debuggable, "0");
    }
    int is_debug = (strcmp(debuggable, "1") == 0);
    
    __system_property_get("ro.crypto.state", encrypted);
    int is_encrypted = (strcmp(encrypted, "encrypted") == 0);
    
    snprintf(buffer, buffer_size,
             "SELinux=%s,Debug=%s,Encrypted=%s",
             selinux,
             is_debug ? "true" : "false",
             is_encrypted ? "true" : "false");
}

/*
 * ========================================================================
 * JNI API for Kotlin/Java
 * ========================================================================
 */

/*
 * Get CPU architecture
 * Returns the primary ABI of the device
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getCpuArchitecture(JNIEnv* env, jobject thiz) {
    char arch[PROP_VALUE_MAX];
    
    // Try ro.product.cpu.abi first (primary ABI)
    int len = __system_property_get("ro.product.cpu.abi", arch);
    
    if (len <= 0) {
        LOGW("Failed to get CPU architecture from system property");
        return (*env)->NewStringUTF(env, "unknown");
    }
    
    LOGI("CPU Architecture: %s", arch);
    return (*env)->NewStringUTF(env, arch);
}

/*
 * Get CPU core count
 * Returns number of available processor cores
 */
JNIEXPORT jint JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getCpuCoreCount(JNIEnv* env, jobject thiz) {
    long cores = sysconf(_SC_NPROCESSORS_CONF);
    
    if (cores <= 0) {
        LOGW("Failed to get CPU core count, defaulting to 1");
        cores = 1;
    }
    
    LOGI("CPU Core Count: %ld", cores);
    return (jint)cores;
}

/*
 * Get sensor list
 * Returns comma-separated list of available sensors
 * Note: This is simplified - full sensor enumeration requires SensorManager JNI calls
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getSensorList(JNIEnv* env, jobject thiz) {
    // For Phase 2, we return a placeholder
    // Full implementation would require calling back into Java SensorManager
    // or using Android NDK sensor APIs (ASensorManager)
    
    LOGI("Sensor enumeration: simplified implementation");
    
    // Common sensors on most devices
    const char* sensor_list = "accelerometer,gyroscope,magnetometer,proximity,light";
    
    return (*env)->NewStringUTF(env, sensor_list);
}

/*
 * Get security status
 * Checks SELinux, encryption, and debug status
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getSecurityStatus(JNIEnv* env, jobject thiz) {
    char selinux[PROP_VALUE_MAX];
    char debuggable[PROP_VALUE_MAX];
    char encrypted[PROP_VALUE_MAX];
    char security_status[512];
    
    // Get SELinux status
    __system_property_get("ro.boot.selinux", selinux);
    if (strlen(selinux) == 0) {
        strcpy(selinux, "unknown");
    }
    
    // Check if device is debuggable (may fail on Android 8.0+ due to SELinux)
    int prop_result = __system_property_get("ro.debuggable", debuggable);
    if (prop_result <= 0) {
        strcpy(debuggable, "0");  // Default to non-debuggable if property unavailable
    }
    int is_debug = (strcmp(debuggable, "1") == 0);
    
    // Check encryption status
    __system_property_get("ro.crypto.state", encrypted);
    int is_encrypted = (strcmp(encrypted, "encrypted") == 0);
    
    // Format security status string
    snprintf(security_status, sizeof(security_status),
             "SELinux=%s,Debug=%s,Encrypted=%s",
             selinux,
             is_debug ? "true" : "false",
             is_encrypted ? "true" : "false");
    
    LOGI("Security Status: %s", security_status);
    return (*env)->NewStringUTF(env, security_status);
}

/*
 * Get build information
 * Returns manufacturer, model, Android version, SDK level
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getBuildInfo(JNIEnv* env, jobject thiz) {
    char manufacturer[PROP_VALUE_MAX];
    char model[PROP_VALUE_MAX];
    char android_version[PROP_VALUE_MAX];
    char sdk_version[PROP_VALUE_MAX];
    char build_info[512];
    
    // Get device manufacturer
    __system_property_get("ro.product.manufacturer", manufacturer);
    if (strlen(manufacturer) == 0) {
        strcpy(manufacturer, "unknown");
    }
    
    // Get device model
    __system_property_get("ro.product.model", model);
    if (strlen(model) == 0) {
        strcpy(model, "unknown");
    }
    
    // Get Android version
    __system_property_get("ro.build.version.release", android_version);
    if (strlen(android_version) == 0) {
        strcpy(android_version, "unknown");
    }
    
    // Get SDK version
    __system_property_get("ro.build.version.sdk", sdk_version);
    if (strlen(sdk_version) == 0) {
        strcpy(sdk_version, "0");
    }
    
    // Format build info
    snprintf(build_info, sizeof(build_info),
             "%s %s Android %s API %s",
             manufacturer, model, android_version, sdk_version);
    
    LOGI("Build Info: %s", build_info);
    return (*env)->NewStringUTF(env, build_info);
}

/*
 * Get full capability report
 * Returns JSON-formatted complete device capability report
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getFullReport(JNIEnv* env, jobject thiz) {
    char arch[PROP_VALUE_MAX];
    char manufacturer[PROP_VALUE_MAX];
    char model[PROP_VALUE_MAX];
    char android_version[PROP_VALUE_MAX];
    char sdk_version[PROP_VALUE_MAX];
    char selinux[PROP_VALUE_MAX];
    char debuggable[PROP_VALUE_MAX];
    char encrypted[PROP_VALUE_MAX];
    char report[2048];
    
    // Gather all information
    __system_property_get("ro.product.cpu.abi", arch);
    long cores = sysconf(_SC_NPROCESSORS_CONF);
    __system_property_get("ro.product.manufacturer", manufacturer);
    __system_property_get("ro.product.model", model);
    __system_property_get("ro.build.version.release", android_version);
    __system_property_get("ro.build.version.sdk", sdk_version);
    __system_property_get("ro.boot.selinux", selinux);
    
    // Handle restricted property access on Android 8.0+
    if (__system_property_get("ro.debuggable", debuggable) <= 0) {
        strcpy(debuggable, "0");
    }
    
    __system_property_get("ro.crypto.state", encrypted);
    
    // Handle missing values
    if (strlen(arch) == 0) strcpy(arch, "unknown");
    if (strlen(manufacturer) == 0) strcpy(manufacturer, "unknown");
    if (strlen(model) == 0) strcpy(model, "unknown");
    if (strlen(android_version) == 0) strcpy(android_version, "unknown");
    if (strlen(sdk_version) == 0) strcpy(sdk_version, "0");
    if (strlen(selinux) == 0) strcpy(selinux, "unknown");
    if (cores <= 0) cores = 1;
    
    int is_debug = (strcmp(debuggable, "1") == 0);
    int is_encrypted = (strcmp(encrypted, "encrypted") == 0);
    
    // Build JSON report
    snprintf(report, sizeof(report),
             "{\n"
             "  \"cpu\": {\n"
             "    \"architecture\": \"%s\",\n"
             "    \"cores\": %ld\n"
             "  },\n"
             "  \"sensors\": [\"accelerometer\", \"gyroscope\", \"magnetometer\", \"proximity\", \"light\"],\n"
             "  \"security\": {\n"
             "    \"selinux\": \"%s\",\n"
             "    \"debug_mode\": %s,\n"
             "    \"encrypted\": %s\n"
             "  },\n"
             "  \"build\": {\n"
             "    \"manufacturer\": \"%s\",\n"
             "    \"model\": \"%s\",\n"
             "    \"android_version\": \"%s\",\n"
             "    \"sdk_level\": %s\n"
             "  }\n"
             "}",
             arch, cores,
             selinux, is_debug ? "true" : "false", is_encrypted ? "true" : "false",
             manufacturer, model, android_version, sdk_version);
    
    LOGI("Generated full capability report");
    return (*env)->NewStringUTF(env, report);
}
