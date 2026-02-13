# Phase 4 — Key Management & Identity Bootstrapping

**Status**: 🟡 In Progress  
**Prerequisites**: Phase 3 (Secure Local Storage) - COMPLETED ✅  
**Started**: 2026-02-12  
**Objective**: Generate device-specific cryptographic identity and establish sovereign key management

---

## Objective

Establish device-specific cryptographic identity:
1. Generate ECC key pair for device identity
2. Store keys securely using Phase 3 infrastructure
3. Implement device attestation capability
4. Verify identity operations functional
5. Foundation for sovereign authentication and networking

**This is NOT about**:
- Building a full PKI system
- Implementing certificate authorities
- User authentication (that's later)

**This IS about**:
- Proving device can have sovereign identity
- Key generation in native layer
- Identity persistence across sessions
- Foundation for Phase 5 (networking with mTLS)

---

## What Will Be Implemented

### 1. ECC Cryptography

**Algorithm**: Ed25519 (EdDSA)

**Why Ed25519**:
- **Modern**: State-of-the-art elliptic curve
- **Fast**: Faster than RSA, faster than ECDSA
- **Small**: 32-byte private keys, 32-byte public keys, 64-byte signatures
- **Secure**: 128-bit security level, no known vulnerabilities
- **Deterministic**: Same message always produces same signature (unlike ECDSA)
- **Widely Used**: SSH, TLS 1.3, Signal Protocol, WireGuard
- **Implementable**: ~500 lines of C code for full sovereignty

**Alternative Considered**: P-256 (NIST curve)
- **Rejected**: Patent concerns, NSA design, more complex, less performant

### 2. Sovereign Ed25519 Implementation

**Files**:
- `app/src/main/cpp/sovereign_ed25519.h` - Ed25519 API
- `app/src/main/cpp/sovereign_ed25519.c` - Full implementation from RFC 8032

**Operations**:
```c
// Key generation
int ed25519_generate_keypair(uint8_t public_key[32], uint8_t private_key[32]);

// Signing
int ed25519_sign(const uint8_t* message, size_t message_len,
                const uint8_t private_key[32],
                uint8_t signature[64]);

// Verification
int ed25519_verify(const uint8_t* message, size_t message_len,
                  const uint8_t signature[64],
                  const uint8_t public_key[32]);
```

**Dependencies**: SHA-512 (will implement sovereign version)

### 3. Device Identity Management

**File**: `app/src/main/cpp/device_identity.c`

**Identity Structure**:
```c
typedef struct {
    uint8_t public_key[32];    // Ed25519 public key
    uint8_t private_key[32];   // Ed25519 private key (encrypted at rest)
    uint64_t created_timestamp;
    char device_id[64];        // Human-readable device ID
    uint8_t fingerprint[32];   // SHA-256 of public key
} device_identity_t;
```

**Functions**:
```c
// Generate new device identity
int generate_device_identity(device_identity_t* identity);

// Store identity (encrypted with Phase 3 storage)
int store_device_identity(const device_identity_t* identity);

// Load identity from storage
int load_device_identity(device_identity_t* identity);

// Get public key fingerprint (for display/logging)
void get_identity_fingerprint(const device_identity_t* identity, char* hex_output);

// Sign data with device identity
int sign_with_device_identity(const device_identity_t* identity,
                              const uint8_t* data, size_t len,
                              uint8_t signature[64]);

// Verify device attestation
int verify_device_attestation(const uint8_t public_key[32],
                              const uint8_t* data, size_t len,
                              const uint8_t signature[64]);
```

### 4. Key Storage

**Storage Strategy**:
- Private key encrypted with ChaCha20-Poly1305 (Phase 3)
- Stored in `/data/data/com.sovereigndroid.core/files/secure/device_identity.enc`
- Public key can be exposed (it's public)
- Fingerprint logged for debugging

**File Format**:
```
[ENCRYPTED SECTION: ChaCha20-Poly1305]
  - 32 bytes: Ed25519 private key
  - 8 bytes: created_timestamp
  - 64 bytes: device_id
[END ENCRYPTED]
[PLAINTEXT SECTION]
  - 32 bytes: Ed25519 public key
  - 32 bytes: fingerprint (SHA-256 of public key)
```

### 5. Kotlin JNI Bridge

**File**: `app/src/main/java/com/sovereigndroid/core/DeviceIdentity.kt`

```kotlin
object DeviceIdentity {
    // Generate new device identity (creates new key pair)
    external fun generateIdentity(): Boolean
    
    // Load existing identity from storage
    external fun loadIdentity(): Boolean
    
    // Check if identity exists
    external fun hasIdentity(): Boolean
    
    // Get public key (hex-encoded)
    external fun getPublicKey(): String?
    
    // Get fingerprint (hex-encoded SHA-256 of public key)
    external fun getFingerprint(): String?
    
    // Get device ID (human-readable)
    external fun getDeviceId(): String?
    
    // Sign data with device identity
    external fun signData(data: ByteArray): ByteArray?
    
    // Verify signature
    external fun verifySignature(data: ByteArray, signature: ByteArray, publicKey: ByteArray): Boolean
    
    // Create attestation (sign device info)
    fun createAttestation(): Attestation?
}

data class Attestation(
    val deviceId: String,
    val timestamp: Long,
    val publicKeyFingerprint: String,
    val signature: ByteArray,
    val deviceInfo: String  // JSON from Phase 2
)
```

### 6. MainActivity Integration

Add Phase 4 test to `MainActivity.onCreate()`:
```kotlin
// Phase 4: Device Identity
initializeDeviceIdentity()
```

New test function:
```kotlin
private fun initializeDeviceIdentity() {
    Log.i(TAG, "=== Phase 4: Device Identity ===")
    
    if (!DeviceIdentity.hasIdentity()) {
        Log.i(TAG, "No identity found, generating new...")
        DeviceIdentity.generateIdentity()
    } else {
        Log.i(TAG, "Identity found, loading...")
        DeviceIdentity.loadIdentity()
    }
    
    val publicKey = DeviceIdentity.getPublicKey()
    val fingerprint = DeviceIdentity.getFingerprint()
    val deviceId = DeviceIdentity.getDeviceId()
    
    Log.i(TAG, "Device ID: $deviceId")
    Log.i(TAG, "Public Key: ${publicKey?.take(16)}...")
    Log.i(TAG, "Fingerprint: $fingerprint")
    
    // Test signing
    val testData = "SovereignDroid Phase 4 Test".toByteArray()
    val signature = DeviceIdentity.signData(testData)
    
    if (signature != null) {
        Log.i(TAG, "✅ Signature created (${signature.size} bytes)")
        
        // Test verification
        val publicKeyBytes = hexToBytes(publicKey!!)
        val verified = DeviceIdentity.verifySignature(testData, signature, publicKeyBytes)
        Log.i(TAG, if (verified) "✅ Signature verified" else "❌ Signature verification failed")
    }
    
    Log.i(TAG, "=== Phase 4: SUCCESS ===")
}
```

---

## Technical Approach

### Ed25519 Implementation

**Based on**: RFC 8032 (Edwards-Curve Digital Signature Algorithm)

**Curve**: Curve25519 (y² = x³ + 486662x² + x over GF(2^255 - 19))

**Components to Implement**:
1. **SHA-512**: For key derivation and signing
2. **Curve arithmetic**: Point addition, scalar multiplication
3. **Key generation**: From 32 random bytes
4. **Signing**: Deterministic signature generation
5. **Verification**: Public key recovery and verification

**Implementation Size**: ~500-700 lines of C

**Testing**: Use RFC 8032 test vectors

### Sovereign SHA-512

Need to implement SHA-512 for Ed25519 (ChaCha20-Poly1305 didn't need it).

**File**: `app/src/main/cpp/sovereign_sha512.c`

**API**:
```c
typedef struct {
    uint64_t state[8];
    uint64_t count[2];
    uint8_t buffer[128];
} sha512_ctx;

void sha512_init(sha512_ctx* ctx);
void sha512_update(sha512_ctx* ctx, const uint8_t* data, size_t len);
void sha512_final(sha512_ctx* ctx, uint8_t hash[64]);

// Convenience function
void sha512(const uint8_t* data, size_t len, uint8_t hash[64]);
```

---

## Success Criteria

Phase 4 is complete when:

1. ✅ Ed25519 implementation compiles and links
2. ✅ SHA-512 implementation functional
3. ✅ Key pair generation works (32-byte keys)
4. ✅ Signing produces 64-byte signatures
5. ✅ Verification accepts valid signatures
6. ✅ Verification rejects invalid signatures
7. ✅ Identity persists across app restarts
8. ✅ Public key fingerprint displayed
9. ✅ Device attestation created and verified
10. ✅ No external crypto dependencies maintained
11. ✅ Test vectors from RFC 8032 pass

---

## Implementation Plan

### Step 1: SHA-512 Implementation
- Implement SHA-512 from FIPS 180-4
- Test with NIST test vectors
- ~200 lines of C

### Step 2: Ed25519 Core
- Implement curve arithmetic (field operations, point ops)
- Implement scalar multiplication
- ~300 lines of C

### Step 3: Ed25519 Key Operations
- Key generation
- Signing
- Verification
- ~200 lines of C

### Step 4: Device Identity Layer
- Identity struct and management
- Storage integration with Phase 3
- Fingerprint generation
- ~150 lines of C

### Step 5: JNI Bridge
- DeviceIdentity.kt
- Native function mappings
- ~100 lines of Kotlin

### Step 6: Integration & Testing
- MainActivity Phase 4 test
- Verify on device
- Cross-session persistence test

---

## Security Considerations

### Private Key Protection
- Private key encrypted at rest (ChaCha20-Poly1305 from Phase 3)
- Private key never logged
- Private key wiped from memory after use
- Consider memory locking (mlock) for private key

### Key Generation
- Use /dev/urandom (Android secure RNG)
- 32 bytes of entropy for Ed25519 seed
- Verify randomness quality

### Signature Security
- Ed25519 is deterministic (no nonce reuse issues like ECDSA)
- Constant-time operations (timing attack resistant)
- Small attack surface

### Future Enhancements
- Hardware-backed keys (Android KeyStore) - Phase 4.1
- Key rotation policy - Phase 4.2
- Multiple identities (e.g., per-app) - Phase 4.3
- Biometric authentication - Phase 4.4

---

## Testing Strategy

### Unit Tests
- SHA-512 test vectors (NIST FIPS 180-4)
- Ed25519 test vectors (RFC 8032 Appendix A)
- Key generation randomness
- Sign/verify round-trip

### Integration Tests
- Generate identity
- Store and load identity
- Verify persistence across app restart
- Create attestation
- Verify attestation

### Device Tests
- Run on Samsung SM-S938U (test device)
- Verify key generation
- Verify signing performance (< 1ms)
- Verify verification performance (< 1ms)
- Test storage corruption handling

---

## Dependencies

- **Phase 3**: Secure storage for private key encryption
- **Phase 2**: Device info for attestation
- **None external**: Maintaining zero external crypto dependencies

---

## Performance Expectations

Ed25519 is extremely fast:
- **Key generation**: < 0.5ms
- **Signing**: < 0.5ms
- **Verification**: < 1ms

Compare to RSA:
- RSA-2048 signing: 10-50ms
- RSA-2048 verification: 0.5-2ms

Ed25519 is 20-100x faster than RSA for signing, similar verification speed.

---

## Phase 4 Sign-Off

```
Date: ___________
Device: ___________

[ ] Ed25519 implementation compiles
[ ] SHA-512 implementation functional
[ ] RFC 8032 test vectors pass
[ ] Key pair generated successfully
[ ] Signing produces valid signatures
[ ] Verification works correctly
[ ] Identity persists across restarts
[ ] Public key fingerprint correct
[ ] Device attestation functional
[ ] Zero external dependencies maintained
[ ] Performance acceptable (< 1ms per operation)

Phase 4 Status: COMPLETE ✅
```

---

## After Phase 4

**Phase 4 Enables**:
- Device-to-device authentication
- mTLS with device identity (Phase 5)
- Sovereign authentication (no OAuth)
- Signed attestations
- Device reputation system

**Next Recommended**: Phase 5 (Network Stack) using Phase 4 identity for mTLS

---

**Created**: 2026-02-12  
**Objective**: Establish cryptographic device identity  
**Complexity**: High (elliptic curve cryptography)  
**Purpose**: Foundation for sovereign authentication and networking
