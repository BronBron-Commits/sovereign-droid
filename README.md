# SovereignDroid


**Status**: 🟢 Native Character Movement & Facing Implemented  
**Phase**: 3.1 (Native Input & Visual Feedback)  
**Date**: 2026-02-12

---

## What Is This?

SovereignDroid is a **controlled engineering platform** for Android development.

This is not an app framework.  
This is not a feature-complete application.  

This is a **baseline** for building sovereignty-focused capabilities on Android.

---

## New: Character Movement & Facing (2026-02-12)

- Character now visually rotates to face the direction of movement on the ground (native OpenGL renderer)
- Facing direction is smooth and matches both X (red) and Z (blue) axes
- All movement and facing logic is implemented natively in C (see `renderer.c`)
- Touch input sets movement target; character walks and turns correctly

---

---

## Current Status

### ✅ Phase 0.1 - Baseline Verified (2026-02-12)
- Environment contract defined (`DEV_ENV.md`)
- Version baseline locked (`VERSION_LOCK.md`)
- Architecture decisions documented (`ARCHITECTURE.md`)
- Operational workflows standardized and tested
- Deterministic builds confirmed

### ✅ Phase 1 - Native Core Bootstrap Complete (2026-02-12)
- Native library builds (`libsovereign_core.so`)
- JNI integration functional
- Native logging operational
- Platform control validated
- CMake toolchain proven

### ✅ Phase 2 - Device Capability Enumeration (COMPLETE - 2026-02-12)
- Native capability query functions operational
- CPU architecture and core detection verified (arm64-v8a, 8 cores)
- Security status checks functional (SELinux, debug, encryption)
- Build information accurate (Samsung SM-S938U, Android 16, API 36)
- JSON-formatted capability reporting working
- Hardware introspection proven

**Verified on**: Samsung SM-S938U, Android 16 (API 36)

See: [PHASE_2.md](PHASE_2.md) for complete verification results

### � Phase 3 - Secure Local Storage (IMPLEMENTED - READY TO TEST)
- AES-256-GCM authenticated encryption
- Native cryptographic operations (OpenSSL/BoringSSL)
- Encrypted key-value storage in app private storage
- GCM authentication prevents tampering
- Ephemeral key generation (256-bit)
- Round-trip encrypt/decrypt/verify cycle

**Test Phase 3 now**:
```powershell
.\gradlew clean
.\gradlew assembleDebug
adb install -r app\build\outputs\apk\debug\app-debug.apk
adb logcat -c
adb shell am start -n com.sovereigndroid.core/com.sovereigndroid.core.MainActivity
adb logcat -d --pid=$(adb shell pidof com.sovereigndroid.core) | Select-String "Phase 3|SecureStorage"
```

See: [PHASE_3.md](PHASE_3.md) for verification details and tampering tests


### 🔵 Next: Define Phase 4

---

## Quick Start

### Prerequisites

- JDK 17
- Android SDK with platform 36
- Physical Android device (API 24+)
- USB debugging enabled

### First Time Setup

1. **Set environment variables**:
   ```bash
   export JAVA_HOME=/path/to/jdk-17
   export ANDROID_HOME=/path/to/android-sdk
   ```

2. **Validate environment**:
   ```bash
   source env.sh
   ```
   or
   ```bash
   make check
   ```

3. **Connect device**:
   ```bash
   adb devices  # Should show your device
   ```

### Build and Run

```bash
./dev.sh
```

or

```bash
make run
```

This will:
1. Build debug APK
2. Install to device
3. Launch application

### View Logs

```bash
./log.sh
```

or

```bash
make log
```

---

## Architecture

### Principles

- **Physical device only** - No emulator support
- **Minimal modules** - Single `app` module initially
- **No Compose** - XML layouts for MVP
- **Controlled dependencies** - Essential AndroidX only
- **Deterministic builds** - Version locked and stable

See: [ARCHITECTURE.md](ARCHITECTURE.md)

---

## Project Structure

```
SovereignDroid/
├── app/                         # Application module
│   └── src/main/
│       ├── AndroidManifest.xml
│       ├── java/com/sovereigndroid/core/
│       │   └── MainActivity.kt
│       └── res/
├── gradle/                      # Gradle configuration
│   ├── libs.versions.toml      # Version catalog
│   └── wrapper/
├── DEV_ENV.md                  # 🔒 Environment contract
├── VERSION_LOCK.md             # 🔒 Version baseline
├── ARCHITECTURE.md             # 🔒 Design decisions
├── BASELINE_VERIFICATION.md    # 🔴 Required next step
├── PHASE_1.md                  # 🔵 After verification
├── env.sh                      # 🔧 Environment setup
├── dev.sh                      # 🚀 Primary workflow
├── clean.sh                    # 🧹 Clean utility
├── log.sh                      # 📊 Logging utility
├── Makefile                    # 📋 Standard interface
├── build.gradle.kts
└── settings.gradle.kts
```

---

## Development Workflow

### Standard Commands

| Command | Purpose |
|---------|---------|
| `./dev.sh` | Build, install, and launch |
| `./clean.sh` | Clean build artifacts |
| `./log.sh` | View application logs |
| `make run` | Same as `./dev.sh` |
| `make build` | Build only |
| `make check` | Validate environment |

### Workflow Rules

1. **Always source environment first**:
   ```bash
   source env.sh
   ```

2. **Use provided scripts** - Do not manually invoke Gradle or adb unless debugging

3. **Clean before major changes**:
   ```bash
   ./clean.sh && ./dev.sh
   ```

4. **Check logs after launch**:
   ```bash
   ./log.sh
   ```

---

## Next Steps for Engineers

### 🔴 Immediate: Phase 0.1

**Verify the baseline before any development.**

Follow: [BASELINE_VERIFICATION.md](BASELINE_VERIFICATION.md)

You must prove:
1. Clean machine setup works
2. Build is deterministic
3. Workflow is reproducible
4. Documentation is accurate

**Do not proceed until verification is complete and signed off.**

---

### � Phase 1 - Native Core Bootstrap

**Status**: ✅ VERIFIED - Phase Complete (2026-02-12)

Follow: [PHASE_1.md](PHASE_1.md)

**Verified Results**:
- ✅ Native library builds and loads (`libsovereign_core.so`)
- ✅ JNI bridge functional (Kotlin ↔ C)
- ✅ CMake toolchain operational
- ✅ Native logging works from C layer
- ✅ No crashes, no JNI errors
- ✅ Platform control validated

**Proven Capabilities**:
- Low-level native code execution
- Kotlin ↔ C communication via JNI
- Native logging infrastructure
- ABI stability (arm64-v8a)

**Platform foundation complete. Ready for Phase 2.**

---

## What NOT to Do

❌ Add features  
❌ Add UI complexity  
❌ Add Compose  
❌ Add networking  
❌ Add CI/CD  
❌ Add modules  
❌ Refactor existing code  
❌ Upgrade dependencies without reason  

**Lock the foundation first.**

---

## Upgrade Policy

All version changes require:
1. Update `VERSION_LOCK.md` first
2. Document reason
3. Re-validate baseline
4. Update `DEV_ENV.md` if environment changes
5. Commit with clear message

See: [VERSION_LOCK.md](VERSION_LOCK.md)

---

## Documentation Index

| Document | Purpose |
|----------|---------|
| [README.md](README.md) | This file - project overview |
| [DEV_ENV.md](DEV_ENV.md) | Environment requirements and setup |
| [VERSION_LOCK.md](VERSION_LOCK.md) | Exact version specifications |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Design decisions and constraints |
| [BASELINE_VERIFICATION.md](BASELINE_VERIFICATION.md) | Phase 0.1 verification steps |
| [PHASE_1.md](PHASE_1.md) | Native core bootstrap specification |

---

## Critical Principle

**You do not build on unverified infrastructure.**

Validate each phase completely before proceeding to the next.

---

## Questions?

1. Read `DEV_ENV.md` for environment setup
2. Read `ARCHITECTURE.md` for design decisions
3. Read `VERSION_LOCK.md` for version policy
4. Follow `BASELINE_VERIFICATION.md` for next steps

---

**Established**: 2026-02-12  
**Status**: Native character movement and facing direction fully implemented and visually correct. Baseline established, verification required for next phases.  
**Philosophy**: Controlled growth, deliberate decisions, proven foundations
