/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: include/umicom/native_launcher/sha256.h
 * Purpose: Incremental SHA-256 for native delivery integrity checks.
 * This is an unkeyed digest, not a signature, authenticator or FIPS validation.
 * The arithmetic follows FIPS 180-4 sections 4.1.2, 4.2.2, 5 and 6.2.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_NATIVE_LAUNCHER_SHA256_H
#define UMICOM_NATIVE_LAUNCHER_SHA256_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NATIVE_SHA256_BYTES 32U
#define UMI_NATIVE_SHA256_HEX_CAPACITY 65U
/** Caller-owned context. Initialise before use; do not share a mutable context
 * between threads. Final consumes the context; initialise again before reuse. */
typedef struct UmiNativeSha256 {
    uint32_t words[8];
    uint64_t totalBytes;
    unsigned char block[64];
    size_t used;
    int finalised;
} UmiNativeSha256;
void UmiNativeSha256Init(UmiNativeSha256 *context);
/** NULL data is accepted only for length zero. Rejects a total >= 2^64 bits. */
UmiStatus UmiNativeSha256Update(UmiNativeSha256 *context, const void *data, size_t length);
UmiStatus UmiNativeSha256Final(UmiNativeSha256 *context, unsigned char outDigest[32]);
UmiStatus UmiNativeSha256Buffer(const void *data, size_t length, char outHex[65]);
void UmiNativeSha256Hex(const unsigned char digest[32], char outHex[65]);
#ifdef __cplusplus
}
#endif
#endif
