/*
 * SovereignDroid Secure Storage - Header
 * 
 * Phase 3: Native-layer encrypted key-value storage
 * AES-256-GCM authenticated encryption
 */

#ifndef SOVEREIGNDROID_SECURE_STORAGE_H
#define SOVEREIGNDROID_SECURE_STORAGE_H

#include <jni.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Native C API for internal use
 */

// Initialize secure storage subsystem
int secure_storage_initialize(void);

// Store binary data with a key
int secure_storage_store(const char* key, const uint8_t* data, size_t data_len);

// Retrieve binary data by key
int secure_storage_retrieve(const char* key, uint8_t* data, size_t data_len);

// Delete data by key
int secure_storage_delete(const char* key);

/*
 * JNI API for Kotlin/Java
 */

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    initialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_SecureStorage_initialize(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    storeSecure
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_SecureStorage_storeSecure(JNIEnv* env, jobject thiz, jstring key, jstring value);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    retrieveSecure
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_SecureStorage_retrieveSecure(JNIEnv* env, jobject thiz, jstring key);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    deleteSecure
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_SecureStorage_deleteSecure(JNIEnv* env, jobject thiz, jstring key);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_SecureStorage_exists(JNIEnv* env, jobject thiz, jstring key);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    clear
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_sovereigndroid_core_SecureStorage_clear(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_SecureStorage
 * Method:    getStoragePath
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_SecureStorage_getStoragePath(JNIEnv* env, jobject thiz);

#ifdef __cplusplus
}
#endif

#endif // SOVEREIGNDROID_SECURE_STORAGE_H
