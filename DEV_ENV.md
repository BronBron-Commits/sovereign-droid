# Development Environment Contract

This document defines the **required** environment configuration for SovereignDroid.

Any deviation from these specifications may result in non-reproducible builds or runtime failures.

---

## Required Versions

### Java Development Kit
- **Required JDK**: 17
- **Minimum**: JDK 17
- **Tested with**: OpenJDK 17 or Oracle JDK 17

Verify:
```bash
java -version
```

### Android Gradle Plugin
- **Version**: 8.13.2
- **Lock Status**: Hard-pinned in `gradle/libs.versions.toml`

### Gradle
- **Version**: 8.13
- **Lock Status**: Hard-pinned in `gradle/wrapper/gradle-wrapper.properties`

### Kotlin
- **Version**: 2.0.21
- **Lock Status**: Hard-pinned in `gradle/libs.versions.toml`

### Android SDK
- **compileSdk**: 36
- **minSdk**: 24
- **targetSdk**: 36

### Required SDK Components
```
platforms;android-36
platform-tools
build-tools;35.0.0
```

Install via:
```bash
sdkmanager "platforms;android-36" "platform-tools" "build-tools;35.0.0"
```

---

## Required Environment Variables

These must be set before any build operation:

```bash
export JAVA_HOME=/path/to/jdk-17
export ANDROID_HOME=/path/to/android-sdk
export ANDROID_SDK_ROOT=$ANDROID_HOME
```

Add to PATH:
```bash
export PATH=$JAVA_HOME/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$PATH
```

**Do not modify these individually.**  
Use `env.sh` at project root.

---

## Required Physical Device

- Android 7.0 (API 24) or higher
- USB debugging enabled
- Connected via ADB

Verify:
```bash
adb devices
```

**No emulator support.**  
SovereignDroid targets real hardware only.

---

## Prohibited Modifications

The following are **not allowed** without engineering review:

- Changing JDK version
- Upgrading AGP without VERSION_LOCK.md update
- Upgrading Gradle without VERSION_LOCK.md update
- Adding new SDK dependencies without ARCHITECTURE.md review
- Changing minSdk below 24
- Adding emulator configurations

---

## Version Drift Prevention

If any version changes:

1. Update `VERSION_LOCK.md` first
2. Document reason in commit message
3. Re-validate on clean machine
4. Update this file

---

## Validation Checklist

Before first build:

- [ ] JDK 17 installed and in PATH
- [ ] ANDROID_HOME set correctly
- [ ] `env.sh` sourced
- [ ] Android SDK platform 36 installed
- [ ] Physical device connected
- [ ] `adb devices` shows device

If all checked, run:
```bash
./dev.sh
```

---

**Last Updated**: 2026-02-12  
**Baseline Version**: 1.0
