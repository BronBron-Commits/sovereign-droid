# SovereignDroid Architecture

This document defines the **design decisions** and **engineering constraints** that govern SovereignDroid's development.

**Purpose**: Prevent architectural drift and premature complexity.

---

## Core Principles

### 1. Physical Device Only
**No emulator support.**

**Reason**:
- SovereignDroid targets real-world constraints
- Hardware-backed security features require physical devices
- Emulator introduces abstraction that hides real performance characteristics
- Forces engineers to test on actual deployment targets

### 2. Minimal Module Structure
**Single `app` module initially.**

**Reason**:
- Premature modularization creates coordination overhead
- Feature boundaries are unclear at this stage
- Single module = faster builds during active development
- Modularize only when boundaries are proven and stable

**When to add modules**:
- After 3+ features share clear domain boundaries
- When compilation time exceeds 30 seconds
- When testability requires isolation

### 3. No Jetpack Compose (Initially)
**Using XML layouts and View system.**

**Reason**:
- Compose adds build complexity and learning curve
- XML is sufficient for MVP functionality
- Allows focus on platform fundamentals, not UI framework
- Can migrate later when UI requirements stabilize

**Migration trigger**:
- When dynamic UI becomes a core requirement
- When team has capacity for framework shift

### 4. Controlled Dependency Surface
**Only essential AndroidX libraries.**

**Reason**:
- Each dependency is a liability (security, updates, conflicts)
- Prefer Android SDK APIs over third-party abstractions
- Keep attack surface minimal
- Update friction is proportional to dependency count

**Before adding any dependency**:
1. Can this be implemented with Android SDK APIs?
2. Does this solve a current problem or a hypothetical one?
3. Is this library actively maintained?
4. Update `VERSION_LOCK.md`

### 5. On-Device Validation First
**No CI/CD initially.**

**Reason**:
- Build system stability must be proven locally first
- CI adds another variable during baseline establishment
- Engineers must understand build process intimately
- Automate only after process is repeatable

**CI trigger**:
- After 10+ successful local builds
- When multiple engineers are contributing
- When build process is documented and stable

---

## System Boundaries

### Input Boundary
- User interaction only (no network initially)
- Local device sensors and capabilities
- Android OS services

### Output Boundary
- Local UI only
- Local storage (files, databases)
- No external API calls

### Security Boundary
- Assume device is trusted (user-controlled)
- No server-side validation assumptions
- Data stays on-device

---

## What This Is NOT

❌ **Not a framework**  
SovereignDroid is an application platform, not a reusable SDK

❌ **Not multi-platform**  
Android only. No iOS, no web, no desktop

❌ **Not feature-complete**  
This is a baseline. Features are added deliberately

❌ **Not dependency-driven**  
We don't add libraries to solve hypothetical problems

---

## Technology Decisions

### Build System
- **Gradle** with Kotlin DSL
- **Why**: Industry standard, well-documented, IDE-supported

### Language
- **Kotlin** 100%
- **Why**: Modern, null-safe, concise, first-class Android support

### UI Layer
- **XML layouts + ViewBinding**
- **Why**: Simple, debuggable, low overhead

### Threading
- **Coroutines** (when needed)
- **Why**: Kotlin-native, structured concurrency

### Storage
- **TBD** (will be defined when needed)
- Options: SharedPreferences, Room, raw files

### Networking
- **None initially**
- When needed: OkHttp + Retrofit (industry standard)

---

## Build Automation Philosophy

Engineers should run:
```bash
./dev.sh
```

This single command:
1. Sources environment
2. Builds APK
3. Installs to device
4. Launches app

**No IDE dependence.**  
Android Studio is a tool, not a requirement.

---

## Future Architecture Considerations

The following are **explicitly deferred**:

- Multi-module structure
- NDK/JNI layer
- Dependency injection framework (Dagger/Hilt)
- Navigation component
- Jetpack Compose migration
- Remote data sources
- Background workers (WorkManager)
- Content providers

These will be added **only when needed**, not preemptively.

---

## Validation Requirements

Before any architectural change:

1. Does this solve a current problem?
2. Can we prove it with a failing test or metric?
3. Does the benefit justify the added complexity?
4. Have we documented the decision here?

If any answer is "no", defer the change.

---

## Architecture Review Triggers

Update this document when:

- Adding first network call
- Adding first database layer
- Adding second module
- Introducing new framework (Compose, Navigation, etc.)
- Changing threading model
- Adding background processing

---

**Established**: 2026-02-12  
**Philosophy**: Simplicity, clarity, deliberate growth  
**Status**: Baseline locked
