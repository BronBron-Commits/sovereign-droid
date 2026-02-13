# Version Lock

This document establishes the **exact versions** of all build-critical components in SovereignDroid.

All upgrades must be **deliberate** and documented here.

---

## Build System

| Component | Version | Lock Date | Status |
|-----------|---------|-----------|--------|
| Gradle | 8.13 | 2026-02-12 | 🔒 Locked |
| Android Gradle Plugin (AGP) | 8.13.2 | 2026-02-12 | 🔒 Locked |
| Kotlin | 2.0.21 | 2026-02-12 | 🔒 Locked |
| CMake | 3.22.1 | 2026-02-12 | 🔒 Locked |
| NDK | r26 (AGP default) | 2026-02-12 | 🔒 Locked |

---

## Android SDK

| Component | Version | Reason |
|-----------|---------|--------|
| compileSdk | 36 | Latest stable |
| targetSdk | 36 | Latest stable |
| minSdk | 24 (Android 7.0) | Minimum for modern security |
| Build Tools | 35.0.0 | AGP default |

---

## Runtime

| Component | Version | Status |
|-----------|---------|--------|
| JDK | 17 | 🔒 Required |
| JVM Target | 11 | Current (migrate to 17 later) |
| C Standard | C11 | 🔒 Locked |
| Target ABIs | arm64-v8a, armeabi-v7a | Primary + Legacy support |

---

## Core Dependencies

| Library | Version | Purpose | Lock Status |
|---------|---------|---------|-------------|
| androidx.core:core-ktx | 1.17.0 | Kotlin extensions | 🔒 Locked |
| androidx.appcompat | 1.7.1 | Compatibility layer | 🔒 Locked |
| material | 1.13.0 | Material components | 🔒 Locked |
| androidx.activity | 1.12.3 | Activity handling | 🔒 Locked |
| androidx.constraintlayout | 2.2.1 | Layout system | 🔒 Locked |

---

## Test Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| junit | 4.13.2 | Unit testing |
| androidx.test.ext:junit | 1.3.0 | Android testing |
| androidx.test.espresso:espresso-core | 3.7.0 | UI testing |

---

## Upgrade Policy

### When to Upgrade

- Critical security patch
- Blocking bug fix
- Deliberate feature requirement

### When NOT to Upgrade

- "Keep dependencies fresh"
- Automated bot suggestions
- Minor version bumps without purpose

### Upgrade Process

1. Update this document first
2. Document reason in **Reason for Change**
3. Test on physical device
4. Update `DEV_ENV.md` if environment changes
5. Commit with clear message: `[VERSION] Upgrade X from Y to Z - [reason]`

---

## Reason for ChaPhase 1: Native Core Bootstrap
- Added CMake 3.22.1 for native build
- Added NDK r26 (AGP default) for JNI support
- Locked C++17 standard for native code
- Locked arm64-v8a as primary target ABI
- Native integration proven and functional

### 2026-02-12 - nge Log

### 2026-02-12 - Initial Lock
- Established baseline versions
- Locked current stable versions across toolchain
- No changes, initial capture

---

**Baseline Established**: 2026-02-12  
**Last Reviewed**: 2026-02-12  
**Next Review**: Before adding new modules or major features
