/*
 * SovereignDroid Ed25519 Implementation
 * 
 * Based on RFC 8032 (Edwards-Curve Digital Signature Algorithm)
 * Curve: Edwards25519 (-x^2 + y^2 = 1 + dx^2y^2 mod p)
 * where p = 2^255 - 19, d = -121665/121666
 * 
 * Key size: 32 bytes
 * Signature size: 64 bytes
 */

#ifndef SOVEREIGN_ED25519_H
#define SOVEREIGN_ED25519_H

#include <stdint.h>
#include <stddef.h>

#define ED25519_PRIVATE_KEY_SIZE 32
#define ED25519_PUBLIC_KEY_SIZE 32
#define ED25519_SIGNATURE_SIZE 64
#define ED25519_SEED_SIZE 32

/*
 * Generate Ed25519 key pair from random seed
 * seed: 32 random bytes (from secure RNG)
 * public_key: output 32-byte public key
 * private_key: output 32-byte private key
 */
void ed25519_create_keypair(uint8_t public_key[32], uint8_t private_key[32], const uint8_t seed[32]);

/*
 * Sign a message with Ed25519
 * message: data to sign
 * message_len: length of message
 * private_key: 32-byte private key
 * public_key: 32-byte public key
 * signature: output 64-byte signature
 */
void ed25519_sign(uint8_t signature[64], const uint8_t* message, size_t message_len,
                  const uint8_t private_key[32], const uint8_t public_key[32]);

/*
 * Verify an Ed25519 signature
 * signature: 64-byte signature
 * message: data that was signed
 * message_len: length of message
 * public_key: 32-byte public key
 * Returns: 1 if signature is valid, 0 if invalid
 */
int ed25519_verify(const uint8_t signature[64], const uint8_t* message, size_t message_len,
                   const uint8_t public_key[32]);

#endif // SOVEREIGN_ED25519_H
