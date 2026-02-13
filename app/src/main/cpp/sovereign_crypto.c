/*
 * SovereignDroid Cryptography - Implementation
 * 
 * ChaCha20-Poly1305 authenticated encryption (RFC 8439)
 * Implemented from specification - no external dependencies
 * 
 * This is sovereignty crypto: every operation is under our control.
 */

#include "sovereign_crypto.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// ============================================================================
// ChaCha20 Stream Cipher Implementation
// ============================================================================

// ChaCha20 quarter round operation
#define ROTL32(v, n) (((v) << (n)) | ((v) >> (32 - (n))))

#define QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = ROTL32(d, 16); \
    c += d; b ^= c; b = ROTL32(b, 12); \
    a += b; d ^= a; d = ROTL32(d, 8);  \
    c += d; b ^= c; b = ROTL32(b, 7);

// Convert little-endian bytes to uint32_t
static uint32_t load32_le(const uint8_t* src) {
    return ((uint32_t)src[0]) |
           ((uint32_t)src[1] << 8) |
           ((uint32_t)src[2] << 16) |
           ((uint32_t)src[3] << 24);
}

// Convert uint32_t to little-endian bytes
static void store32_le(uint8_t* dst, uint32_t val) {
    dst[0] = val & 0xFF;
    dst[1] = (val >> 8) & 0xFF;
    dst[2] = (val >> 16) & 0xFF;
    dst[3] = (val >> 24) & 0xFF;
}

/*
 * ChaCha20 block function
 * Generates 64 bytes of keystream from key, nonce, and counter
 */
static void chacha20_block(uint32_t out[16], const uint8_t key[32],
                          const uint8_t nonce[12], uint32_t counter) {
    uint32_t state[16];
    int i;
    
    // Initialize state with constants, key, counter, nonce
    // Constants: "expand 32-byte k"
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    // Key (256 bits = 8 words)
    for (i = 0; i < 8; i++) {
        state[4 + i] = load32_le(key + i * 4);
    }
    
    // Counter (1 word)
    state[12] = counter;
    
    // Nonce (96 bits = 3 words)
    for (i = 0; i < 3; i++) {
        state[13 + i] = load32_le(nonce + i * 4);
    }
    
    // Copy initial state to output
    memcpy(out, state, 64);
    
    // Perform 20 rounds (10 double rounds)
    for (i = 0; i < 10; i++) {
        // Column rounds
        QUARTERROUND(out[0], out[4], out[8],  out[12]);
        QUARTERROUND(out[1], out[5], out[9],  out[13]);
        QUARTERROUND(out[2], out[6], out[10], out[14]);
        QUARTERROUND(out[3], out[7], out[11], out[15]);
        
        // Diagonal rounds
        QUARTERROUND(out[0], out[5], out[10], out[15]);
        QUARTERROUND(out[1], out[6], out[11], out[12]);
        QUARTERROUND(out[2], out[7], out[8],  out[13]);
        QUARTERROUND(out[3], out[4], out[9],  out[14]);
    }
    
    // Add original state to output
    for (i = 0; i < 16; i++) {
        out[i] += state[i];
    }
}

/*
 * ChaCha20 encryption/decryption
 */
void chacha20_encrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                     const uint8_t nonce[CHACHA20_NONCE_SIZE],
                     uint32_t counter,
                     const uint8_t* input,
                     uint8_t* output,
                     size_t length) {
    uint32_t keystream[16];
    uint8_t* ks_bytes = (uint8_t*)keystream;
    size_t i, j;
    
    for (i = 0; i < length; i += 64) {
        // Generate 64-byte keystream block
        chacha20_block(keystream, key, nonce, counter++);
        
        // XOR with input
        for (j = 0; j < 64 && i + j < length; j++) {
            output[i + j] = input[i + j] ^ ks_bytes[j];
        }
    }
}

// ============================================================================
// Poly1305 Message Authentication Code Implementation
// ============================================================================

/*
 * Poly1305 authenticator
 * Computes MAC tag to detect tampering
 */
void poly1305_authenticate(const uint8_t key[32],
                          const uint8_t* message,
                          size_t length,
                          uint8_t tag[POLY1305_TAG_SIZE]) {
    // Poly1305 state: accumulator and key
    uint32_t r[5], h[5], s[4];
    uint32_t c;
    size_t i, j;
    
    // Load r from key (with clamping)
    r[0] = (load32_le(key + 0) & 0x0FFFFFFF);
    r[1] = (load32_le(key + 4) & 0x0FFFFFFC);
    r[2] = (load32_le(key + 8) & 0x0FFFFFFC);
    r[3] = (load32_le(key + 12) & 0x0FFFFFFC);
    
    // Load s from key
    for (i = 0; i < 4; i++) {
        s[i] = load32_le(key + 16 + i * 4);
    }
    
    // Initialize accumulator h = 0
    memset(h, 0, sizeof(h));
    
    // Process message in 16-byte blocks
    for (i = 0; i < length; ) {
        uint32_t block[4] = {0};
        size_t block_size = (length - i > 16) ? 16 : (length - i);
        
        // Load block (little-endian)
        for (j = 0; j < block_size; j++) {
            block[j / 4] |= ((uint32_t)message[i + j]) << ((j % 4) * 8);
        }
        
        // Add 0x01 byte after message (padding)
        block[block_size / 4] |= ((uint32_t)1) << ((block_size % 4) * 8);
        
        // h += block
        c = 0;
        for (j = 0; j < 4; j++) {
            uint64_t sum = (uint64_t)h[j] + block[j] + c;
            h[j] = sum & 0xFFFFFFFF;
            c = sum >> 32;
        }
        h[4] += c;
        
        // h *= r (modulo 2^130 - 5)
        uint64_t mul[5] = {0};
        for (j = 0; j < 4; j++) {
            for (size_t k = 0; k < 4; k++) {
                mul[j + k] += (uint64_t)h[j] * r[k];
            }
        }
        
        // Reduce modulo 2^130 - 5
        c = 0;
        for (j = 0; j < 5; j++) {
            uint64_t sum = mul[j] + c;
            h[j] = sum & 0xFFFFFFFF;
            c = sum >> 32;
        }
        
        // Reduce high bits (* 5)
        c = h[4] >> 2;
        h[4] &= 3;
        h[0] += c * 5;
        
        i += block_size;
    }
    
    // Final reduction and add s
    // h += s
    c = 0;
    for (i = 0; i < 4; i++) {
        uint64_t sum = (uint64_t)h[i] + s[i] + c;
        h[i] = sum & 0xFFFFFFFF;
        c = sum >> 32;
    }
    
    // Output tag (little-endian)
    for (i = 0; i < 4; i++) {
        store32_le(tag + i * 4, h[i]);
    }
}

// ============================================================================
// ChaCha20-Poly1305 AEAD (Authenticated Encryption with Associated Data)
// ============================================================================

/*
 * ChaCha20-Poly1305 encrypt
 */
int chacha20_poly1305_encrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                              const uint8_t nonce[CHACHA20_NONCE_SIZE],
                              const uint8_t* plaintext,
                              size_t plaintext_len,
                              uint8_t* ciphertext,
                              uint8_t tag[POLY1305_TAG_SIZE]) {
    uint8_t poly_key[32];
    
    // Generate Poly1305 key from ChaCha20 block 0
    memset(poly_key, 0, 32);
    chacha20_encrypt(key, nonce, 0, poly_key, poly_key, 32);
    
    // Encrypt plaintext with ChaCha20 (starting from counter = 1)
    chacha20_encrypt(key, nonce, 1, plaintext, ciphertext, plaintext_len);
    
    // Compute Poly1305 MAC over ciphertext
    poly1305_authenticate(poly_key, ciphertext, plaintext_len, tag);
    
    // Zero poly_key for security
    memset(poly_key, 0, 32);
    
    return 1;
}

/*
 * ChaCha20-Poly1305 decrypt
 */
int chacha20_poly1305_decrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                              const uint8_t nonce[CHACHA20_NONCE_SIZE],
                              const uint8_t* ciphertext,
                              size_t ciphertext_len,
                              const uint8_t tag[POLY1305_TAG_SIZE],
                              uint8_t* plaintext) {
    uint8_t poly_key[32];
    uint8_t computed_tag[POLY1305_TAG_SIZE];
    int i, tag_match;
    
    // Generate Poly1305 key from ChaCha20 block 0
    memset(poly_key, 0, 32);
    chacha20_encrypt(key, nonce, 0, poly_key, poly_key, 32);
    
    // Verify Poly1305 MAC over ciphertext
    poly1305_authenticate(poly_key, ciphertext, ciphertext_len, computed_tag);
    
    // Constant-time tag comparison (prevent timing attacks)
    tag_match = 0;
    for (i = 0; i < POLY1305_TAG_SIZE; i++) {
        tag_match |= (tag[i] ^ computed_tag[i]);
    }
    
    // Zero sensitive data
    memset(poly_key, 0, 32);
    memset(computed_tag, 0, POLY1305_TAG_SIZE);
    
    if (tag_match != 0) {
        // Authentication failed - data tampered
        memset(plaintext, 0, ciphertext_len);
        return 0;
    }
    
    // Decrypt ciphertext with ChaCha20 (starting from counter = 1)
    chacha20_encrypt(key, nonce, 1, ciphertext, plaintext, ciphertext_len);
    
    return 1;
}

// ============================================================================
// Random Number Generation
// ============================================================================

/*
 * Generate cryptographically secure random bytes
 * Uses /dev/urandom (Android's secure RNG)
 */
int sovereign_random_bytes(uint8_t* buffer, size_t length) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return 0;
    }
    
    ssize_t result = read(fd, buffer, length);
    close(fd);
    
    return (result == (ssize_t)length) ? 1 : 0;
}
