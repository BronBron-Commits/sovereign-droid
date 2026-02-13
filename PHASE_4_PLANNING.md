# Phase 4 — Planning & Future Enhancements

**Status**: 🔵 Planning  
**Prerequisites**: Phase 3 (Secure Local Storage) - COMPLETED ✅  
**Current Date**: 2026-02-12

---

## Completed Foundation

After Phase 3, we have proven:
- ✅ Native code execution (Phase 1)
- ✅ Hardware introspection (Phase 2)
- ✅ Sovereign cryptography (Phase 3)

**Key Achievement**: Complete sovereignty over security-critical code with zero external crypto dependencies.

---

## Identified Improvements

### 1. SELinux Security Handling

**Current State**: 
- Detection shows "SELinux=permissive" on test device
- Basic detection implemented in Phase 2

**Issues**:
- Permissive mode may indicate security policy not enforcing
- Need robust handling for security-sensitive operations
- Should validate SELinux state before critical operations

**Proposed Enhancement**:
```c
// Enhanced SELinux validation
typedef enum {
    SELINUX_ENFORCING,   // Full policy enforcement
    SELINUX_PERMISSIVE,  // Logging only, no enforcement
    SELINUX_DISABLED,    // Completely disabled
    SELINUX_UNKNOWN      // Cannot determine
} selinux_state_t;

// Function to check if SELinux context is safe for crypto operations
int validate_selinux_for_crypto(void);

// Function to get detailed SELinux policy info
const char* get_selinux_policy_version(void);
```

**Security Considerations**:
- Detect if device is rooted or bootloader unlocked
- Check if SELinux is enforcing for production builds
- Validate file contexts for secure storage directory
- Warn if security posture is degraded

**Priority**: Medium (important for production deployments)

---

### 2. Persistent Encrypted Storage

**Current State**:
- Phase 3 uses ephemeral 256-bit keys (per-session)
- Keys regenerated on each app restart
- No persistent user data storage yet

**Proposed Enhancement**: Android KeyStore Integration

**Why KeyStore**:
- Hardware-backed key storage (TEE/Secure Element)
- Keys never leave secure hardware
- Authenticated encryption keys
- Biometric authentication support

**Implementation Plan**:
```
Phase 4a: KeyStore Integration
- Generate persistent keys in Android KeyStore
- Use StrongBox if available (dedicated secure chip)
- Require user authentication for key access
- Key rotation policy

Phase 4b: Biometric Authentication
- Fingerprint/Face unlock for key access
- Fallback to PIN/password
- Attempt counter and lockout

Phase 4c: Real User Data
- Persistent preferences storage
- Encrypted configuration
- User secrets (tokens, credentials)
- App state preservation
```

**API Design**:
```kotlin
object PersistentStorage {
    // Initialize with KeyStore-backed key
    fun initializeWithKeyStore(keyAlias: String, requireAuth: Boolean): Boolean
    
    // Store with biometric authentication
    fun storeWithAuth(key: String, value: String, cipher: Cipher): Boolean
    
    // Retrieve with biometric authentication
    fun retrieveWithAuth(key: String, cipher: Cipher): String?
    
    // Key rotation
    fun rotateKey(oldAlias: String, newAlias: String): Boolean
}
```

**Priority**: High (required for production user data)

---

### 3. Enhanced Sensor Enumeration

**Current State**:
- Basic sensor list (accelerometer, gyroscope, magnetometer, proximity, light)
- String-based reporting

**Proposed Enhancement**: Detailed Sensor Capabilities

**Extended Sensor Data**:
```c
typedef struct {
    char name[64];
    char vendor[64];
    int type;
    float max_range;
    float resolution;
    float power_mah;
    int min_delay_us;
    int max_delay_us;
    int fifo_max_event_count;
    int fifo_reserved_event_count;
    bool is_wakeup;
} sensor_info_t;

// Get detailed sensor information
sensor_info_t* enumerate_sensors_detailed(int* count);

// Check for specific sovereignty-relevant sensors
bool has_secure_sensor(const char* sensor_type);
```

**Sovereignty Use Cases**:
- Direct sensor access bypassing Android framework
- Raw IMU data for navigation without Google
- Magnetometer for compass without network
- Barometer for altitude without GPS

**Priority**: Medium (useful for offline capabilities)

---

### 4. Logging Improvements

**Current State**:
- Basic Android logging via `__android_log_print`
- All logs visible in system logcat
- No structured logging

**Proposed Enhancement**: Sovereign Logging System

**Features**:
```c
// Log levels with security awareness
typedef enum {
    LOG_TRACE,    // Development only
    LOG_DEBUG,    // Verbose debugging
    LOG_INFO,     // Informational
    LOG_WARN,     // Warnings
    LOG_ERROR,    // Errors
    LOG_CRITICAL, // Critical security events
    LOG_AUDIT     // Security audit trail (encrypted)
} log_level_t;

// Structured logging with security filtering
void sovereign_log(log_level_t level, const char* category, 
                  const char* format, ...);

// Encrypted audit log (never goes to Android logcat)
void audit_log(const char* event, const char* context);

// Retrieve encrypted audit logs
char* get_audit_trail(uint64_t since_timestamp);
```

**Security Benefits**:
- Sensitive data never logged to system
- Encrypted audit trail for forensics
- Tamper-evident log chain
- Sovereign log storage (not Android's logcat)

**Priority**: High (critical for production security)

---

### 5. Process Isolation & Integrity

**Current State**:
- Runs in standard Android app process
- No integrity checking
- No isolation from framework

**Proposed Enhancement**: Enhanced Security Posture

**Integrity Checking**:
```c
// Verify native library hasn't been tampered with
bool verify_native_library_integrity(void);

// Check if debugger is attached
bool is_debugger_attached(void);

// Detect emulator or rooted device
bool is_running_on_compromised_device(void);

// Verify app signature matches expected
bool verify_app_signature(JNIEnv* env, jobject context);
```

**Memory Protection**:
- Secure memory allocation for crypto keys
- Memory locking (mlock) for sensitive data
- Guard pages around key material
- Stack canaries and ASLR validation

**Priority**: High (defense in depth)

---

### 6. Hardware-Backed Security (TEE)

**Current State**:
- Using software crypto (ChaCha20-Poly1305)
- No TEE integration

**Proposed Enhancement**: Trusted Execution Environment

**TEE Capabilities**:
- Check for ARM TrustZone support
- Detect available secure hardware
- Use hardware crypto engines
- Secure boot chain validation

**Implementation**:
```c
// Check if TEE is available
bool has_trusted_execution_environment(void);

// Get TEE type (TrustZone, SGX, etc.)
const char* get_tee_type(void);

// Execute crypto in hardware
int tee_encrypt(const uint8_t* plaintext, size_t len,
               uint8_t* ciphertext, uint8_t* tag);
```

**Priority**: Medium (hardware acceleration, enhanced security)

---

## Phase 4 Recommendation

Based on sovereignty priorities and current foundation:

**Phase 4: Persistent Secure Storage + Logging**

Combine enhancements #2 and #4:
1. Android KeyStore integration for persistent keys
2. Sovereign logging system (encrypted audit trail)
3. Real user data storage (encrypted preferences)

**Why This Combination**:
- Natural evolution from Phase 3 ephemeral storage
- Logging needed before deploying to users
- Both required for production-ready platform
- Moderate complexity (doable in one phase)

**Success Criteria**:
- Keys persist across app restarts
- User data encrypted with KeyStore-backed keys
- Sensitive operations never logged to Android logcat
- Encrypted audit trail retrievable
- Biometric authentication optional but supported

---

## Long-Term Roadmap

### Phase 5 Options:
- Direct sensor access (bypass framework)
- TEE integration (hardware security)
- Process isolation (enhanced integrity)

### Phase 6 Options:
- Network stack sovereignty (bypass Android networking)
- Custom permission system (finer-grained than Android)
- Backup/restore with encryption

### Phase 7+:
- Multi-device sync (encrypted)
- Peer-to-peer communication (no cloud)
- Offline maps and navigation

---

## Decision Point

**Choose Phase 4 Focus**:

- **Option A**: Persistent Storage + Logging (recommended)
- **Option B**: SELinux Hardening + Integrity Checking
- **Option C**: Sensor Enhancement + Direct Access
- **Option D**: Custom combination

---

**Document Created**: 2026-02-12  
**Purpose**: Plan Phase 4 based on Phase 1-3 learnings  
**Status**: Awaiting user input for Phase 4 selection
