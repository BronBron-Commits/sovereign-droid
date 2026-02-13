/*
 * SovereignDroid Native Core - Implementation
 * 
 * Phase 1: Minimal native functions to prove JNI integration.
 * 
 * Purpose:
 * - Validate native compilation
 * - Validate JNI loading
 * - Validate native logging
 * - Validate ABI stability
 * 
 * This is NOT feature code.
 * This is infrastructure validation.
 */

#include "sovereign_core.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "SovereignCore"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Native core version - increment when native layer changes
#define NATIVE_CORE_VERSION 1

/*
 * Bootstrap message function
 * Proves that:
 * - Native code executes
 * - String creation works
 * - Logging works
 * - JNI bridge is functional
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_NativeCore_getBootstrapMessage(JNIEnv* env, jobject thiz) {
    LOGI("Native core bootstrap initiated");
    LOGI("JNI interface functional");
    LOGI("Native logging operational");
    
    const char* message = "SovereignDroid Native Core: Active [v1]";
    
    jstring result = (*env)->NewStringUTF(env, message);
    
    if (result == NULL) {
        LOGE("Failed to create Java string from native");
        return (*env)->NewStringUTF(env, "ERROR: String creation failed");
    }
    
    LOGI("Bootstrap message created successfully");
    return result;
}

/*
 * Native version function
 * Returns the version number of the native core
 * Useful for debugging and version tracking
 */
JNIEXPORT jint JNICALL
Java_com_sovereigndroid_core_NativeCore_getNativeVersion(JNIEnv* env, jobject thiz) {
    LOGI("Native version query: %d", NATIVE_CORE_VERSION);
    return NATIVE_CORE_VERSION;
}

/*
 * ==================================================================
 * Phase 4: Device Identity JNI Functions
 * ==================================================================
 */

#include "device_identity.h"

// Initialize device identity subsystem
JNIEXPORT jint JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_initialize(JNIEnv* env, jobject thiz) {
    return device_identity_init();
}

// Check if identity exists
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_hasIdentity(JNIEnv* env, jobject thiz) {
    return (jboolean)device_identity_exists();
}

// Generate new identity
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_generateIdentity(JNIEnv* env, jobject thiz) {
    return (jboolean)(device_identity_generate() == IDENTITY_OK);
}

// Load existing identity
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_loadIdentity(JNIEnv* env, jobject thiz) {
    return (jboolean)(device_identity_load() == IDENTITY_OK);
}

// Get public key
JNIEXPORT jbyteArray JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_getPublicKey(JNIEnv* env, jobject thiz) {
    uint8_t public_key[32];
    
    if (device_identity_get_public_key(public_key) != IDENTITY_OK) {
        return NULL;
    }
    
    jbyteArray result = (*env)->NewByteArray(env, 32);
    if (result == NULL) return NULL;
    
    (*env)->SetByteArrayRegion(env, result, 0, 32, (jbyte*)public_key);
    return result;
}

// Get fingerprint
JNIEXPORT jbyteArray JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_getFingerprint(JNIEnv* env, jobject thiz) {
    uint8_t fingerprint[32];
    
    if (device_identity_get_fingerprint(fingerprint) != IDENTITY_OK) {
        return NULL;
    }
    
    jbyteArray result = (*env)->NewByteArray(env, 32);
    if (result == NULL) return NULL;
    
    (*env)->SetByteArrayRegion(env, result, 0, 32, (jbyte*)fingerprint);
    return result;
}

// Sign data
JNIEXPORT jbyteArray JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_signData(JNIEnv* env, jobject thiz, jbyteArray data) {
    if (data == NULL) return NULL;
    
    jsize data_len = (*env)->GetArrayLength(env, data);
    jbyte* data_bytes = (*env)->GetByteArrayElements(env, data, NULL);
    
    if (data_bytes == NULL) return NULL;
    
    uint8_t signature[64];
    int result = device_identity_sign((uint8_t*)data_bytes, data_len, signature);
    
    (*env)->ReleaseByteArrayElements(env, data, data_bytes, JNI_ABORT);
    
    if (result != IDENTITY_OK) return NULL;
    
    jbyteArray sig_array = (*env)->NewByteArray(env, 64);
    if (sig_array == NULL) return NULL;
    
    (*env)->SetByteArrayRegion(env, sig_array, 0, 64, (jbyte*)signature);
    return sig_array;
}

// Verify signature
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_verifySignature(JNIEnv* env, jobject thiz,
                                                            jbyteArray data, jbyteArray signature) {
    if (data == NULL || signature == NULL) return JNI_FALSE;
    
    jsize data_len = (*env)->GetArrayLength(env, data);
    jsize sig_len = (*env)->GetArrayLength(env, signature);
    
    if (sig_len != 64) return JNI_FALSE;
    
    jbyte* data_bytes = (*env)->GetByteArrayElements(env, data, NULL);
    jbyte* sig_bytes = (*env)->GetByteArrayElements(env, signature, NULL);
    
    if (data_bytes == NULL || sig_bytes == NULL) {
        if (data_bytes) (*env)->ReleaseByteArrayElements(env, data, data_bytes, JNI_ABORT);
        if (sig_bytes) (*env)->ReleaseByteArrayElements(env, signature, sig_bytes, JNI_ABORT);
        return JNI_FALSE;
    }
    
    int result = device_identity_verify((uint8_t*)data_bytes, data_len, (uint8_t*)sig_bytes);
    
    (*env)->ReleaseByteArrayElements(env, data, data_bytes, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, signature, sig_bytes, JNI_ABORT);
    
    return (jboolean)(result == 1);
}

// Create attestation
JNIEXPORT jbyteArray JNICALL
Java_com_sovereigndroid_core_DeviceIdentity_createAttestation(JNIEnv* env, jobject thiz) {
    uint8_t attestation[128];
    
    int length = device_identity_create_attestation(attestation, sizeof(attestation));
    if (length < 0) return NULL;
    
    jbyteArray result = (*env)->NewByteArray(env, length);
    if (result == NULL) return NULL;
    
    (*env)->SetByteArrayRegion(env, result, 0, length, (jbyte*)attestation);
    return result;
}
