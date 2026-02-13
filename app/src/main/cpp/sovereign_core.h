/*
 * SovereignDroid Native Core - Header
 * 
 * Minimal native interface for platform control validation.
 * This is Phase 1: Prove JNI integration works.
 */

#ifndef SOVEREIGNDROID_SOVEREIGN_CORE_H
#define SOVEREIGNDROID_SOVEREIGN_CORE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_sovereigndroid_core_NativeCore
 * Method:    getBootstrapMessage
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_NativeCore_getBootstrapMessage(JNIEnv* env, jobject thiz);

/*
 * Class:     com_sovereigndroid_core_NativeCore
 * Method:    getNativeVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_sovereigndroid_core_NativeCore_getNativeVersion(JNIEnv* env, jobject thiz);

#ifdef __cplusplus
}
#endif

#endif // SOVEREIGNDROID_SOVEREIGN_CORE_H
