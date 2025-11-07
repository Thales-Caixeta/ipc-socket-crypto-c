#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include "common.h"
#include "crypto.h"

#define DEFAULT_PORT 5050
#define MAX_MSG 65536

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len; ++i) printf("%02X ", data[i]);
    printf("\n");
}

int main(int argc, char **argv) {
    const char *host = (argc >= 2) ? argv[1] : "127.0.0.1";
    int port = (argc >= 3) ? atoi(argv[2]) : DEFAULT_PORT;
    const char *key = (argc >= 4) ? argv[3] : "chave-secreta-demo";

    printf("[CLIENTE] Conectando em %s:%d\n", host, port);
    printf("[CLIENTE] Digite a mensagem:\n");

    char *line = NULL;
    size_t cap = 0;
    ssize_t n = getline(&line, &cap, stdin);
    if (n < 0) { fprintf(stderr, "Erro ao ler stdin.\n"); free(line); return 1; }
    if (n > 0 && line[n-1] == '\n') line[--n] = '\0';

    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd < 0) { perror("socket"); free(line); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        struct hostent *he = gethostbyname(host);
        if (!he) { fprintf(stderr, "Host inválido.\n"); close(cfd); free(line); return 1; }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    if (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(cfd);
        free(line);
        return 1;
    }

    uint32_t mlen = (uint32_t)strlen(line);
    uint8_t *buf = malloc(mlen);
    memcpy(buf, line, mlen);
    encrypt(buf, mlen, (const uint8_t *)key, strlen(key));

    print_hex("[CLIENTE] Mensagem criptografada para envio", buf, mlen);

    send_frame(cfd, buf, mlen);
    printf("[CLIENTE] Mensagem enviada.\n");

    // recebe texto claro
    void *plain = NULL; uint32_t plain_len = 0;
    recv_frame(cfd, &plain, &plain_len);
    printf("[CLIENTE] ECHO-PLAIN (sem descriptografar): %.*s\n", (int)plain_len, (char *)plain);

    // recebe cifrado e decifra
    void *enc = NULL; uint32_t enc_len = 0;
    recv_frame(cfd, &enc, &enc_len);
    print_hex("[CLIENTE] ECHO-ENCRYPTED recebido (criptografado)", (uint8_t *)enc, enc_len);
    decrypt((uint8_t *)enc, enc_len, (const uint8_t *)key, strlen(key));
    printf("[CLIENTE] ECHO-ENCRYPTED (decifrado): %.*s\n", (int)enc_len, (char *)enc);

    free(enc);
    free(plain);
    free(buf);
    free(line);
    close(cfd);
    return 0;
}
