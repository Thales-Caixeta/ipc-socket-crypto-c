#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
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
    const char *key = "chave-secreta-demo";
    int port = (argc >= 2) ? atoi(argv[1]) : DEFAULT_PORT;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { perror("socket"); return 1; }
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { perror("bind"); close(sfd); return 1; }
    if (listen(sfd, 8) < 0) { perror("listen"); close(sfd); return 1; }

    printf("[SERVIDOR] Escutando em 0.0.0.0:%d\n", port);

    for (;;) {
        struct sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);
        int cfd = accept(sfd, (struct sockaddr *)&caddr, &clen);
        if (cfd < 0) { perror("accept"); continue; }

        printf("\n[SERVIDOR] Cliente conectado.\n");

        void *cipher = NULL;
        uint32_t cipher_len = 0;
        if (recv_frame(cfd, &cipher, &cipher_len) < 0) {
            fprintf(stderr, "Falha ao receber frame.\n");
            close(cfd);
            continue;
        }

        print_hex("[SERVIDOR] Mensagem recebida (criptografada)", (uint8_t *)cipher, cipher_len);

        // decifra
        decrypt((uint8_t *)cipher, cipher_len, (const uint8_t *)key, strlen(key));
        printf("[SERVIDOR] Mensagem decifrada: %.*s\n", (int)cipher_len, (char *)cipher);

        // envia respostas
        const char *plain_prefix = "ECHO-PLAIN: ";
        const char *enc_prefix   = "ECHO-ENCRYPTED: ";

        uint32_t plain_len = (uint32_t)(strlen(plain_prefix) + cipher_len);
        uint32_t enc_len   = (uint32_t)(strlen(enc_prefix) + cipher_len);

        uint8_t *plain = malloc(plain_len);
        uint8_t *enc   = malloc(enc_len);

        memcpy(plain, plain_prefix, strlen(plain_prefix));
        memcpy(plain + strlen(plain_prefix), cipher, cipher_len);

        memcpy(enc, enc_prefix, strlen(enc_prefix));
        memcpy(enc + strlen(enc_prefix), cipher, cipher_len);

        // manda o texto claro
        send_frame(cfd, plain, plain_len);
        printf("[SERVIDOR] Enviado ECHO-PLAIN (texto claro).\n");

        // cifra e manda o cifrado
        encrypt(enc, enc_len, (const uint8_t *)key, strlen(key));
        print_hex("[SERVIDOR] ECHO-ENCRYPTED (criptografado para envio)", enc, enc_len);
        send_frame(cfd, enc, enc_len);
        printf("[SERVIDOR] Enviado ECHO-ENCRYPTED (cifrado).\n");

        free(enc);
        free(plain);
        free(cipher);
        close(cfd);
        printf("[SERVIDOR] Conexão encerrada.\n");
    }

    close(sfd);
    return 0;
}
