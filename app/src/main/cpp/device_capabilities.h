/*
 * SovereignDroid Device Capabilities - Header
 * 
 * Phase 2: Device capability enumeration at native layer
 * Queries hardware, security, and build information
 */

#ifndef SOVEREIGNDROID_DEVICE_CAPABILITIES_H
#define SOVEREIGNDROID_DEVICE_CAPABILITIES_H

#include <jni.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Native C API for internal use
 */

// Initialize device capabilities subsystem
void device_capabilities_init(void);

// Get CPU architecture
void device_capabilities_get_cpu_arch(char* buffer, size_t buffer_size);

// Get CPU core count
int device_capabilities_get_cpu_cores(void);

// Get sensor list
void device_capabilities_get_sensors(char* buffer, size_t buffer_size);

// Get security status
void device_capabilities_get_security(char* buffer, size_t buffer_size);

/*
 * JNI API for Kotlin/Java
 */

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getCpuArchitecture
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getCpuArchitecture(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getCpuCoreCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getCpuCoreCount(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getSensorList
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getSensorList(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getSecurityStatus
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getSecurityStatus(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getBuildInfo
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getBuildInfo(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_DeviceCapabilities
 * Method:    getFullReport
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_DeviceCapabilities_getFullReport(JNIEnv* env, jobject thiz);

#ifdef __cplusplus
}
#endif

#endif // SOVEREIGNDROID_DEVICE_CAPABILITIES_H
