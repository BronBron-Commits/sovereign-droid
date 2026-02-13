# SovereignDroid Native Build Configuration
# CMake configuration for native core library
# Full Native Activity - No Java/Kotlin UI

cmake_minimum_required(VERSION 3.22.1)

# Project declaration
project("sovereign_core" C)

# Add android_native_app_glue for NativeActivity support
# This is required for android_main() entry point
add_library(app-glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(app-glue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue)

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
    ${ANDROID_NDK}/sources/android/native_app_glue)

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
