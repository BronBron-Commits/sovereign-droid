/*
 * SovereignDroid Device Identity Management
 * 
 * Manages device-specific cryptographic identity using Ed25519
 * Identity persists across app restarts using secure storage
 */

#ifndef DEVICE_IDENTITY_H
#define DEVICE_IDENTITY_H

#include <stdint.h>
#include <stddef.h>

// Identity status codes
#define IDENTITY_OK 0
#define IDENTITY_ERROR -1
#define IDENTITY_NOT_FOUND -2
#define IDENTITY_INVALID -3

/*
 * Initialize device identity subsystem
 * Must be called before other identity operations
 */
int device_identity_init(void);

/*
 * Check if device identity exists
 * Returns: 1 if exists, 0 if not
 */
int device_identity_exists(void);

/*
 * Generate new device identity
 * Creates Ed25519 keypair and stores securely
 * Returns: IDENTITY_OK on success, IDENTITY_ERROR on failure
 */
int device_identity_generate(void);

/*
 * Load existing device identity
 * Returns: IDENTITY_OK on success, error code on failure
 */
int device_identity_load(void);

/*
 * Get device public key
 * public_key: output buffer (32 bytes)
 * Returns: IDENTITY_OK on success, error code on failure
 */
int device_identity_get_public_key(uint8_t public_key[32]);

/*
 * Get device identity fingerprint (SHA-256 of public key)
 * fingerprint: output buffer (32 bytes)
 * Returns: IDENTITY_OK on success, error code on failure
 */
int device_identity_get_fingerprint(uint8_t fingerprint[32]);

/*
 * Sign data with device identity
 * data: data to sign
 * data_len: length of data
 * signature: output buffer (64 bytes)
 * Returns: IDENTITY_OK on success, error code on failure
 */
int device_identity_sign(const uint8_t* data, size_t data_len, uint8_t signature[64]);

/*
 * Verify signature with device public key
 * data: data that was signed
 * data_len: length of data
 * signature: signature to verify (64 bytes)
 * Returns: 1 if valid, 0 if invalid, negative on error
 */
int device_identity_verify(const uint8_t* data, size_t data_len, const uint8_t signature[64]);

/*
 * Create device attestation (signed device info)
 * attestation: output buffer (variable size)
 * max_len: maximum output length
 * Returns: actual attestation length, or negative on error
 */
int device_identity_create_attestation(uint8_t* attestation, size_t max_len);

#endif // DEVICE_IDENTITY_H
