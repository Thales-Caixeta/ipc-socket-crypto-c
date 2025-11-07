#include "crypto.h"
#include <stdint.h>
#include <stddef.h>

static uint32_t derive_seed(const uint8_t *key, size_t keylen) {
    uint32_t hash = 0x811C9DC5u;
    for (size_t i = 0; i < keylen; i++) {
        hash ^= key[i];
        hash *= 0x01000193u;
    }
    if (hash == 0) hash = 0xA5A5A5A5u;
    return hash;
}

void ks_create_state(uint32_t *state_out, const uint8_t *key, size_t keylen) {
    *state_out = derive_seed(key, keylen);
    // "queima" algumas iterações
    for (int i = 0; i < 8; ++i) {
        uint32_t x = *state_out;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state_out = x ? x : 0x6C8E9CF5u;
    }
}

uint8_t ks_next_from_state(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x ? x : 0x6C8E9CF5u;
    return (uint8_t)(*state & 0xFF);
}

void encrypt(uint8_t *buf, size_t len, const uint8_t *key, size_t keylen) {
    uint32_t state;
    ks_create_state(&state, key, keylen);
    for (size_t i = 0; i < len; ++i) buf[i] ^= ks_next_from_state(&state);
}

void decrypt(uint8_t *buf, size_t len, const uint8_t *key, size_t keylen) {
    // simétrico: mesma operação
    encrypt(buf, len, key, keylen);
}
