// June 29, 2019
// June 23, 2024

#include "../include/hash_op.h"
#include <config.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <unistd.h>

int file_sha256(int fd, char sum[SHA256_LEN]) {
    static const char HEX[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static EVP_MD_CTX * ctx;

    if (ctx == NULL) {
        ctx = EVP_MD_CTX_create();
    }

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

    char buffer[BUFFER_SIZE];
    ssize_t count;

    while ((count = read(fd, buffer, BUFFER_SIZE)) > 0) {
        EVP_DigestUpdate(ctx, buffer, count);
    }

    if (count == -1) {
        return -1;
    }

    unsigned char md[SHA256_DIGEST_LENGTH];
    EVP_DigestFinal_ex(ctx, md, NULL);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        // sprintf(sum + i * 2, "%02x", md[i]);
        sum[i * 2] = HEX[md[i] >> 4];
        sum[i * 2 + 1] = HEX[md[i] & 0xF];
    }

    sum[SHA256_LEN - 1] = '\0';
    return 0;
}
