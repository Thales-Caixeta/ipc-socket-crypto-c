#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

// Cria uma chave/estado interno a partir da key
void ks_create_state(uint32_t *state_out, const uint8_t *key, size_t keylen);

// Gera next byte de keystream dado state (usa xorshift32)
uint8_t ks_next_from_state(uint32_t *state);

// Encripta in-place: chamada pública
// (funciona simetricamente: chamar decrypt é a mesma operação)
void encrypt(uint8_t *buf, size_t len, const uint8_t *key, size_t keylen);

// Desencripta in-place (wrapper semântico)
void decrypt(uint8_t *buf, size_t len, const uint8_t *key, size_t keylen);

#endif
