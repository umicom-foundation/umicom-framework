/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/sha256.h
 * PURPOSE: Share incremental SHA-256 without depending on launchers or platform services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_SHA256_H
#define UMICOM_BASE_SHA256_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_SHA256_BYTES 32U
#define UMI_SHA256_HEX_CAPACITY 65U
    /** Caller-owned digest state. Initialise it before use; never share mutable
     * state between threads. A digest detects content changes, not a trusted signer. */
    typedef struct UmiSha256
    {
        uint32_t words[8];
        uint64_t totalBytes;
        unsigned char block[64];
        size_t used;
        int finalised;
    } UmiSha256;
    /** Start a fresh digest. NULL is a no-op. Reinitialise after Final to reuse it. */
    void UmiSha256Init(UmiSha256 *context);
    /** Append bytes in order. NULL data is allowed only for zero length.
     * A total exceeding the SHA-256 bit-length limit is refused before mutation. */
    UmiStatus UmiSha256Update(UmiSha256 *context, const void *data, size_t length);
    /** Finish once and write 32 bytes. Invalid state leaves the output unchanged. */
    UmiStatus UmiSha256Final(UmiSha256 *context, unsigned char outDigest[32]);
    /** Render a digest as 64 lowercase hexadecimal characters and a terminator.
     * Input and output must not overlap; NULL input or output is a no-op. */
    void UmiSha256Hex(const unsigned char digest[32], char outHex[65]);
    /** Hash one buffer. Failure clears outHex when it is non-NULL.
     * This is unkeyed SHA-256, not encryption, a signature or a credential vault. */
    UmiStatus UmiSha256Buffer(const void *data, size_t length, char outHex[65]);
#ifdef __cplusplus
}
#endif
#endif
