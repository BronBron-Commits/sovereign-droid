Below is a complete, professional README you can paste directly into `README.md`.

---

# SovereignDroid

Native Android engine built without Java UI layers, emulators, or virtual machines.
SovereignDroid is a C/C++–driven Android NativeActivity application designed to provide a minimal, high-control rendering and input stack.

---

## Overview

SovereignDroid is a fully native Android application built using:

* Android NDK
* NativeActivity
* OpenGL ES
* CMake
* Gradle (as build orchestrator only)

There is no Java UI layer. No Jetpack. No XML layouts.
Rendering, input, and lifecycle handling are controlled entirely from native code.

This project exists to:

* Build a minimal Android-native game/runtime engine
* Maintain strict compiler hygiene (`-Wall -Werror`)
* Provide deterministic rendering control
* Avoid heavy framework abstraction

---

## Architecture

```
Android System
    ↓
NativeActivity (Android framework entry point)
    ↓
android_main()
    ↓
Core Engine Loop
    ↓
Renderer / Input / Game Systems
```

### Core Components

| Component        | Responsibility                   |
| ---------------- | -------------------------------- |
| `android_main()` | Application entry                |
| Renderer         | OpenGL ES context, frame drawing |
| Input System     | Touch & event handling           |
| Engine Loop      | Update + Render cycle            |
| CMake Build      | Native compilation               |
| Gradle           | APK packaging only               |

Strict compilation flags are enforced:

```
-Wall -Werror
```

Warnings fail the build by design.

---

## Build Requirements

* Windows 10/11 (tested)
* Android Studio
* Android SDK
* Android NDK
* ADB configured in PATH

---

## Build Instructions (PowerShell)

From project root:

```powershell
.\gradlew.bat clean assembleDebug
```

Install to device:

```powershell
adb install -r app\build\outputs\apk\debug\app-debug.apk
```

Launch:

```powershell
adb shell am start -n com.sovereigndroid.core/android.app.NativeActivity
```

Clear logs before debugging:

```powershell
adb logcat -c
```

View logs:

```powershell
adb logcat -s SovereignNative:* SovereignRenderer:* SovereignInput:*
```

---

## Project Structure

```
SovereignDroid/
│
├── app/
│   └── src/main/
│       ├── cpp/            # Native engine source
│       ├── AndroidManifest.xml
│       └── CMakeLists.txt
│
├── gradle/                 # Gradle wrapper
├── build.gradle.kts
├── settings.gradle.kts
├── ARCHITECTURE.md
├── ROADMAP.md
└── PHASE_*.md
```

---

## Development Principles

* No emulator-first design (device-native testing)
* No unnecessary dependencies
* No database backends
* No external engines
* Explicit memory control
* Strict compiler enforcement
* Transparent rendering pipeline

---

## Roadmap

Planned progression:

* Phase 1 – Window + Render Loop Baseline
* Phase 2 – Stable Frame Draw + Logging
* Phase 3 – Input Handling
* Phase 4 – Camera + World System
* Phase 5 – Character System
* Future – Scene Graph, Asset Pipeline, Game Logic Layer

See `ROADMAP.md` and phase documents for detailed planning.

---

## Current Status

Engine baseline operational:

* NativeActivity lifecycle functional
* EGL context initialization
* Render loop active
* Logging framework established

Active development:

* Character rendering
* Camera configuration (isometric angle)
* Engine modularization

---

## Philosophy

SovereignDroid is designed to remove abstraction layers and reclaim full control over the Android execution environment.

It prioritizes:

* Determinism
* Performance clarity
* Compiler strictness
* Minimal runtime overhead

---

## License

Specify your license here (MIT recommended if open-source).

---

If you want, I can also provide:

* A cleaner “public-facing” version
* A more technical engine-developer version
* A minimalist hacker-style version
* A polished GitHub-featured version with badges
