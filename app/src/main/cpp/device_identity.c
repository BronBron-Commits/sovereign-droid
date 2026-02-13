/*
 * SovereignDroid Device Identity Management Implementation
 */

#include "device_identity.h"
#include "sovereign_ed25519.h"
#include "sovereign_sha512.h"
#include "sovereign_crypto.h"
#include "secure_storage.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "DeviceIdentity"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Identity storage keys
#define IDENTITY_KEY_PRIVATE "device_identity_private"
#define IDENTITY_KEY_PUBLIC "device_identity_public"

// Current identity (loaded in memory)
static struct {
    uint8_t private_key[ED25519_PRIVATE_KEY_SIZE];
    uint8_t public_key[ED25519_PUBLIC_KEY_SIZE];
    int loaded;
} g_identity = {0};

int device_identity_init(void) {
    LOGI("Initializing device identity subsystem");
    LOGI("Identity algorithm: Ed25519 (RFC 8032)");
    LOGI("Key size: %d bytes private, %d bytes public", 
         ED25519_PRIVATE_KEY_SIZE, ED25519_PUBLIC_KEY_SIZE);
    
    g_identity.loaded = 0;
    return IDENTITY_OK;
}

int device_identity_exists(void) {
    uint8_t temp[ED25519_PUBLIC_KEY_SIZE];
    return (secure_storage_retrieve(IDENTITY_KEY_PUBLIC, temp, sizeof(temp)) == 0) ? 1 : 0;
}

int device_identity_generate(void) {
    LOGI("Generating new device identity...");
    
    // Generate random seed
    uint8_t seed[ED25519_SEED_SIZE];
    if (!sovereign_random_bytes(seed, sizeof(seed))) {
        LOGE("Failed to generate random seed");
        return IDENTITY_ERROR;
    }
    
    // Create Ed25519 keypair
    ed25519_create_keypair(g_identity.public_key, g_identity.private_key, seed);
    
    // Store private key securely (encrypted)
    if (secure_storage_store(IDENTITY_KEY_PRIVATE, g_identity.private_key, 
                            ED25519_PRIVATE_KEY_SIZE) != 0) {
        LOGE("Failed to store private key");
        memset(&g_identity, 0, sizeof(g_identity));
        return IDENTITY_ERROR;
    }
    
    // Store public key (plaintext is fine)
    if (secure_storage_store(IDENTITY_KEY_PUBLIC, g_identity.public_key, 
                            ED25519_PUBLIC_KEY_SIZE) != 0) {
        LOGE("Failed to store public key");
        memset(&g_identity, 0, sizeof(g_identity));
        return IDENTITY_ERROR;
    }
    
    g_identity.loaded = 1;
    
    // Log fingerprint
    uint8_t fingerprint[32];
    sha512(g_identity.public_key, ED25519_PUBLIC_KEY_SIZE, fingerprint);
    
    LOGI("Device identity generated successfully");
    LOGI("Public key: %02x%02x%02x%02x...%02x%02x%02x%02x",
         g_identity.public_key[0], g_identity.public_key[1],
         g_identity.public_key[2], g_identity.public_key[3],
         g_identity.public_key[28], g_identity.public_key[29],
         g_identity.public_key[30], g_identity.public_key[31]);
    LOGI("Fingerprint: %02x%02x%02x%02x...%02x%02x%02x%02x",
         fingerprint[0], fingerprint[1], fingerprint[2], fingerprint[3],
         fingerprint[28], fingerprint[29], fingerprint[30], fingerprint[31]);
    
    return IDENTITY_OK;
}

int device_identity_load(void) {
    LOGI("Loading device identity...");
    
    // Load private key
    if (secure_storage_retrieve(IDENTITY_KEY_PRIVATE, g_identity.private_key,
                               ED25519_PRIVATE_KEY_SIZE) != 0) {
        LOGE("Failed to load private key");
        return IDENTITY_NOT_FOUND;
    }
    
    // Load public key
    if (secure_storage_retrieve(IDENTITY_KEY_PUBLIC, g_identity.public_key,
                               ED25519_PUBLIC_KEY_SIZE) != 0) {
        LOGE("Failed to load public key");
        memset(&g_identity, 0, sizeof(g_identity));
        return IDENTITY_NOT_FOUND;
    }
    
    g_identity.loaded = 1;
    LOGI("Device identity loaded successfully");
    
    return IDENTITY_OK;
}

int device_identity_get_public_key(uint8_t public_key[32]) {
    if (!g_identity.loaded) {
        LOGE("Identity not loaded");
        return IDENTITY_ERROR;
    }
    
    memcpy(public_key, g_identity.public_key, ED25519_PUBLIC_KEY_SIZE);
    return IDENTITY_OK;
}

int device_identity_get_fingerprint(uint8_t fingerprint[32]) {
    if (!g_identity.loaded) {
        LOGE("Identity not loaded");
        return IDENTITY_ERROR;
    }
    
    // Fingerprint = SHA-512 of public key (first 32 bytes)
    uint8_t hash[64];
    sha512(g_identity.public_key, ED25519_PUBLIC_KEY_SIZE, hash);
    memcpy(fingerprint, hash, 32);
    
    return IDENTITY_OK;
}

int device_identity_sign(const uint8_t* data, size_t data_len, uint8_t signature[64]) {
    if (!g_identity.loaded) {
        LOGE("Identity not loaded");
        return IDENTITY_ERROR;
    }
    
    LOGI("Signing %zu bytes of data", data_len);
    
    // Sign with Ed25519
    ed25519_sign(signature, data, data_len, g_identity.private_key, g_identity.public_key);
    
    LOGI("Signature generated: %02x%02x%02x%02x...%02x%02x%02x%02x",
         signature[0], signature[1], signature[2], signature[3],
         signature[60], signature[61], signature[62], signature[63]);
    
    return IDENTITY_OK;
}

int device_identity_verify(const uint8_t* data, size_t data_len, const uint8_t signature[64]) {
    if (!g_identity.loaded) {
        LOGE("Identity not loaded");
        return IDENTITY_ERROR;
    }
    
    // Verify with Ed25519
    int result = ed25519_verify(signature, data, data_len, g_identity.public_key);
    
    LOGI("Signature verification: %s", result ? "PASS" : "FAIL");
    
    return result;
}

int device_identity_create_attestation(uint8_t* attestation, size_t max_len) {
    if (!g_identity.loaded) {
        LOGE("Identity not loaded");
        return IDENTITY_ERROR;
    }
    
    // Attestation format:
    // [32 bytes public key][32 bytes fingerprint][64 bytes signature of "SovereignDroid.Attestation"]
    
    const char* attestation_message = "SovereignDroid.Attestation";
    size_t message_len = strlen(attestation_message);
    
    size_t required_len = 32 + 32 + 64;
    if (max_len < required_len) {
        LOGE("Attestation buffer too small (need %zu, got %zu)", required_len, max_len);
        return IDENTITY_ERROR;
    }
    
    // Copy public key
    memcpy(attestation, g_identity.public_key, 32);
    
    // Copy fingerprint
    uint8_t fingerprint[32];
    device_identity_get_fingerprint(fingerprint);
    memcpy(attestation + 32, fingerprint, 32);
    
    // Sign attestation message
    uint8_t signature[64];
    device_identity_sign((const uint8_t*)attestation_message, message_len, signature);
    memcpy(attestation + 64, signature, 64);
    
    LOGI("Device attestation created (%zu bytes)", required_len);
    
    return (int)required_len;
}
