# Phase 1 — Native Core Bootstrap

**Status**: ✅ IMPLEMENTED  
**Prerequisites**: Phase 0.1 (Baseline Verification) - COMPLETED  
**Implementation Date**: 2026-02-12

---

## Objective

Prove the platform's capability to:
1. Integrate native code via JNI
2. Execute native functions from Kotlin
3. Log from native layer
4. Maintain ABI stability
5. Validate toolchain integrity

This is **not** about features.  
This is about **architectural proof**.

---

## Why Native Layer First?

SovereignDroid requires low-level control:
- Platform sovereignty implications
- Direct hardware access potential
- Security boundary control
- Foundation for future capabilities

Proving JNI works early validates:
- NDK toolchain is configured correctly
- ABI targeting is stable
- Build system handles native compilation
- No integration surprises later

---

## Phase 1 Deliverables

### 1. Native Source Structure

Create:
```
app/src/main/cpp/
├── sovereign_core.c
├── sovereign_core.h
└── CMakeLists.txt
```

### 2. Minimal Native Function

**`sovereign_core.c`**:
```c
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "SovereignCore"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

JNIEXPORT jstring JNICALL
Java_com_sovereigndroid_core_NativeCore_getBootstrapMessage(JNIEnv* env, jobject thiz) {
    LOGI("Native core bootstrap successful");
    return (*env)->NewStringUTF(env, "SovereignDroid Native Core: Active");
}
```

### 3. Kotlin Bridge

**`NativeCore.kt`**:
```kotlin
package com.sovereigndroid.core

object NativeCore {
    init {
        System.loadLibrary("sovereign_core")
    }
    
    external fun getBootstrapMessage(): String
}
```

### 4. Integration Proof

Call from `MainActivity`:
```kotlin
override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_main)
    
    val nativeMessage = NativeCore.getBootstrapMessage()
    Log.i("SovereignDroid", nativeMessage)
}
```

### 5. Build System Integration

Update `app/build.gradle.kts`:
```kotlin
android {
    // ... existing config
    
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}
```

---

## Success Criteria

**Phase 1 is complete when**:

1. ✅ Build succeeds without warnings
2. ✅ Native library loads at runtime
3. ✅ Native function executes successfully
4. ✅ Native log appears in `logcat`
5. ✅ Kotlin receives correct string from native
6. ✅ No crashes on launch
7. ✅ ABI stable (arm64-v8a minimum)
8. ✅ Works on clean build (`./clean.sh && ./dev.sh`)

---

## Verification Steps

After implementation:

```bash
# Clean build
./clean.sh

# Full rebuild
./dev.sh

# Check logs - should show native bootstrap message
./log.sh
```

**Expected log output**:
```
I/SovereignCore: Native core bootstrap successful
I/SovereignDroid: SovereignDroid Native Core: Active
```

---

## What This Proves

✅ **Toolchain Integrity** - NDK is configured correctly  
✅ **JNI Viability** - Bridge between Kotlin and C works  
✅ **Native Logging** - Can debug native layer  
✅ **ABI Stability** - Correct architecture targeting  
✅ **Build Determinism** - CMake integration is stable  

---

## What NOT to Add

During Phase 1, explicitly do NOT:
- ❌ Add complex native logic
- ❌ Implement cryptography
- ❌ Add file system operations
- ❌ Implement networking
- ❌ Add multiple native modules
- ❌ Create abstraction layers

**Keep it minimal.**  
**Prove the mechanism works.**  
**Nothing more.**

---

## Phase 1 Testing Procedure

### Step 1: Clean Build
```bash
./clean.sh
./dev.sh
```

### Step 2: Verify Native Library in APK
```powershell
# On Windows, extract and check for .so file
# APK should contain: lib/arm64-v8a/libsovereign_core.so
```

### Step 3: Clear Logs and Launch
```powershell
# Clear logcat
adb logcat -c

# Launch should happen via dev.sh, or manually:
adb shell am start -n com.sovereigndroid.core/com.sovereigndroid.core.MainActivity

# Watch logs
adb logcat | Select-String "Sovereign"
```

### Step 4: Expected Log Output
```
I/SovereignCore: Native core bootstrap initiated
I/SovereignCore: JNI interface functional
I/SovereignCore: Native logging operational
I/SovereignCore: Bootstrap message created successfully
I/SovereignCore: Native version query: 1
I/SovereignDroid: === Phase 1: Native Core Bootstrap ===
I/SovereignDroid: Native Bootstrap: SovereignDroid Native Core: Active [v1]
I/SovereignDroid: Native Core Version: 1
I/SovereignDroid: === Phase 1: SUCCESS ===
I/SovereignDroid: ✅ Native library loaded
I/SovereignDroid: ✅ JNI functions executed
I/SovereignDroid: ✅ Native logging operational
I/SovereignDroid: ✅ Platform control validated
```

### Step 5: Verify Deterministic Build
```bash
# First build
./clean.sh && ./dev.sh
# Note APK hash or size

# Second build
./clean.sh && ./dev.sh
# APK should be identical (same hash)
```

---

## Phase 1 Sign-Off

**Test Results**:

```
Date: 2026-02-12
Engineer: Verified
Device: Physical Android (arm64-v8a)

[✅] Native code compiles
[✅] Native library loads (libsovereign_core.so)
[✅] JNI function executes (both functions)
[✅] Native logs visible (SovereignCore tags confirmed)
[✅] No crashes or UnsatisfiedLinkError
[✅] Clean build successful
[✅] Deterministic rebuild successful
[✅] ABI matches device architecture (arm64-v8a)

Phase 1 Status: COMPLETE ✅
```

**Verified log output confirms all success criteria met.**

---

## After Phase 1

Update `VERSION_LOCK.md` to include:
- CMake version
- NDK version
- Target ABI specifications

Update `ARCHITECTURE.md` to document:
- Native integration decision
- JNI boundary conventions
- Logging strategy

---

## Next Phase

After Phase 1 completion, define:
- **Phase 2 objectives** (TBD based on platform direction)
- Could be: secure storage, capability enumeration, etc.

**Do not preemptively plan beyond Phase 2.**

---

**Created**: 2026-02-12  
**Objective**: Prove native integration works  
**Complexity**: Minimal  
**Purpose**: Foundation validation, not feature delivery
