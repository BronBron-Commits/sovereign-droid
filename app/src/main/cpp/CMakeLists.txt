# SovereignDroid Native Build Configuration
# CMake configuration for native core library
# Full Native Activity - No Java/Kotlin UI

cmake_minimum_required(VERSION 3.22.1)

# Ensure NDK path is set for includes
if(DEFINED ANDROID_NDK)
    set(NDK_PATH ${ANDROID_NDK})
elseif(DEFINED ANDROID_NDK_ROOT)
    set(NDK_PATH ${ANDROID_NDK_ROOT})
elseif(DEFINED CMAKE_ANDROID_NDK)
    set(NDK_PATH ${CMAKE_ANDROID_NDK})
else()
    message(FATAL_ERROR "ANDROID_NDK variable not set. Please set the NDK path.")
endif()
string(REPLACE "\\" "/" NDK_PATH ${NDK_PATH})

message(STATUS "[CMake Debug] Using NDK path: ${NDK_PATH}")

# Project declaration
project("sovereign_core" C)

# Add android_native_app_glue for NativeActivity support
# This is required for android_main() entry point
add_library(app-glue STATIC
    ${NDK_PATH}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(app-glue PUBLIC
    ${NDK_PATH}/sources/android/native_app_glue)

# Create shared library from native source
add_library(
    sovereign_core
    SHARED
    native_activity.c
    sovereign_core.c
    device_capabilities.c
    sovereign_crypto.c
    secure_storage.c
    sovereign_sha512.c
    sovereign_ed25519.c
    device_identity.c
    renderer.c
    input.c
)

# Include android_native_app_glue headers
target_include_directories(sovereign_core PRIVATE
    ${NDK_PATH}/sources/android/native_app_glue
    ${NDK_PATH}/sysroot/usr/include
    ${NDK_PATH}/sysroot/usr/include/android
    ${NDK_PATH}/sysroot/usr/include/GLES3
)

# Find required Android libraries
find_library(log_lib log)
find_library(android_lib android)  # For ANativeWindow
find_library(egl_lib EGL)  # For EGL context
find_library(gles3_lib GLESv3)  # For OpenGL ES 3.0

# Link against Android libraries
# Use whole-archive to ensure ANativeActivity_onCreate is exported
target_link_libraries(
    sovereign_core
    -Wl,--whole-archive app-glue -Wl,--no-whole-archive
    ${log_lib}
    ${android_lib}
    ${egl_lib}
    ${gles3_lib}
)
