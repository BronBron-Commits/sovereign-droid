/*
 * SovereignDroid SHA-512 Implementation
 * 
 * Based on FIPS 180-4 (Secure Hash Standard)
 * Implemented from specification for sovereignty
 * 
 * SHA-512 is required for Ed25519 key generation and signing
 */

#ifndef SOVEREIGN_SHA512_H
#define SOVEREIGN_SHA512_H

#include <stdint.h>
#include <stddef.h>

// SHA-512 context
typedef struct {
    uint64_t state[8];      // Hash state
    uint64_t count[2];      // Bit count (128-bit)
    uint8_t buffer[128];    // Input buffer
} sha512_ctx;

/*
 * Initialize SHA-512 context
 */
void sha512_init(sha512_ctx* ctx);

/*
 * Update SHA-512 with data
 * Can be called multiple times
 */
void sha512_update(sha512_ctx* ctx, const uint8_t* data, size_t len);

/*
 * Finalize SHA-512 and output hash
 */
void sha512_final(sha512_ctx* ctx, uint8_t hash[64]);

/*
 * Convenience function: hash data in one call
 */
void sha512(const uint8_t* data, size_t len, uint8_t hash[64]);

#endif // SOVEREIGN_SHA512_H
