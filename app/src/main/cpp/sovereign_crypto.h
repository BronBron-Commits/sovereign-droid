/*
 * SovereignDroid Cryptography
 * 
 * Custom implementation of ChaCha20-Poly1305 authenticated encryption
 * 
 * Why custom crypto?
 * - Full sovereignty: No external dependencies
 * - Complete auditability: Every line of crypto code in our control
 * - Transparency: No hidden backdoors or vulnerabilities from third parties
 * - Modern design: ChaCha20-Poly1305 is state-of-the-art (TLS 1.3, WireGuard)
 * 
 * ChaCha20-Poly1305 Specification:
 * - ChaCha20: Stream cipher, 256-bit key, 96-bit nonce
 * - Poly1305: MAC (Message Authentication Code), prevents tampering
 * - Combined: AEAD (Authenticated Encryption with Associated Data)
 * 
 * Security Properties:
 * - Confidentiality: Data encrypted with ChaCha20 stream cipher
 * - Authenticity: Poly1305 MAC detects any tampering
 * - Resistance: Immune to timing attacks (constant-time operations)
 * 
 * References:
 * - RFC 8439: ChaCha20 and Poly1305 for IETF Protocols
 * - No code copied - implemented from specification for sovereignty
 */

#ifndef SOVEREIGN_CRYPTO_H
#define SOVEREIGN_CRYPTO_H

#include <stdint.h>
#include <stddef.h>

// Key and nonce sizes
#define CHACHA20_KEY_SIZE 32    // 256 bits
#define CHACHA20_NONCE_SIZE 12  // 96 bits
#define POLY1305_TAG_SIZE 16    // 128 bits

/*
 * ChaCha20 encryption/decryption
 * 
 * Encrypts or decrypts data using ChaCha20 stream cipher.
 * Same function for both operations (XOR with keystream).
 * 
 * @param key 32-byte encryption key
 * @param nonce 12-byte nonce (must be unique for each message with same key)
 * @param counter Block counter (usually 0, or 1 if block 0 used for Poly1305)
 * @param input Input data (plaintext or ciphertext)
 * @param output Output buffer (ciphertext or plaintext)
 * @param length Length of input/output in bytes
 */
void chacha20_encrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                     const uint8_t nonce[CHACHA20_NONCE_SIZE],
                     uint32_t counter,
                     const uint8_t* input,
                     uint8_t* output,
                     size_t length);

/*
 * Poly1305 message authentication code
 * 
 * Computes 16-byte authentication tag for a message.
 * Detects any modification to ciphertext.
 * 
 * @param key 32-byte one-time key (derived from ChaCha20)
 * @param message Message to authenticate
 * @param length Message length in bytes
 * @param tag Output buffer for 16-byte MAC tag
 */
void poly1305_authenticate(const uint8_t key[CHACHA20_KEY_SIZE],
                          const uint8_t* message,
                          size_t length,
                          uint8_t tag[POLY1305_TAG_SIZE]);

/*
 * ChaCha20-Poly1305 AEAD encrypt
 * 
 * Encrypts plaintext and computes authentication tag.
 * 
 * @param key 32-byte encryption key
 * @param nonce 12-byte nonce (must be unique per message)
 * @param plaintext Input plaintext
 * @param plaintext_len Length of plaintext
 * @param ciphertext Output ciphertext (same length as plaintext)
 * @param tag Output authentication tag (16 bytes)
 * @return 1 on success, 0 on error
 */
int chacha20_poly1305_encrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                              const uint8_t nonce[CHACHA20_NONCE_SIZE],
                              const uint8_t* plaintext,
                              size_t plaintext_len,
                              uint8_t* ciphertext,
                              uint8_t tag[POLY1305_TAG_SIZE]);

/*
 * ChaCha20-Poly1305 AEAD decrypt
 * 
 * Verifies authentication tag and decrypts ciphertext.
 * 
 * @param key 32-byte encryption key
 * @param nonce 12-byte nonce (same as used for encryption)
 * @param ciphertext Input ciphertext
 * @param ciphertext_len Length of ciphertext
 * @param tag Authentication tag to verify (16 bytes)
 * @param plaintext Output plaintext (same length as ciphertext)
 * @return 1 on success (tag valid), 0 on authentication failure (tampering detected)
 */
int chacha20_poly1305_decrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                              const uint8_t nonce[CHACHA20_NONCE_SIZE],
                              const uint8_t* ciphertext,
                              size_t ciphertext_len,
                              const uint8_t tag[POLY1305_TAG_SIZE],
                              uint8_t* plaintext);

/*
 * Generate random bytes for keys and nonces
 * Uses /dev/urandom for cryptographically secure randomness
 * 
 * @param buffer Output buffer
 * @param length Number of random bytes to generate
 * @return 1 on success, 0 on error
 */
int sovereign_random_bytes(uint8_t* buffer, size_t length);

#endif // SOVEREIGN_CRYPTO_H
