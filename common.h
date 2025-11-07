#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static inline int send_all(int fd, const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t *)buf;
    while (len > 0) {
        ssize_t n = send(fd, p, len, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) return -1;
        p += n;
        len -= (size_t)n;
    }
    return 0;
}

// protocolo: [u32 big-endian length][payload]
static inline int send_frame(int fd, const void *buf, uint32_t len) {
    uint32_t netlen = htonl(len);
    if (send_all(fd, &netlen, sizeof(netlen)) < 0) return -1;
    if (len == 0) return 0;
    return send_all(fd, buf, len);
}

static inline int recv_all(int fd, void *buf, size_t len) {
    uint8_t *p = (uint8_t *)buf;
    while (len > 0) {
        ssize_t n = recv(fd, p, len, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) return -1;
        p += n;
        len -= (size_t)n;
    }
    return 0;
}

static inline int recv_frame(int fd, void **out_buf, uint32_t *out_len) {
    uint32_t netlen = 0;
    if (recv_all(fd, &netlen, sizeof(netlen)) < 0) return -1;
    uint32_t len = ntohl(netlen);
    void *buf = NULL;
    if (len > 0) {
        buf = malloc(len);
        if (!buf) return -1;
        if (recv_all(fd, buf, len) < 0) {
            free(buf);
            return -1;
        }
    }
    *out_buf = buf;
    *out_len = len;
    return 0;
}

#endif
