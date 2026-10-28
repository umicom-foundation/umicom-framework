/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/hmac_sha256.c
 *
 * PURPOSE:
 *   Construct standard HMAC-SHA-256 from the established Framework SHA-256
 *   API, with bounded stack memory and failure-atomic caller output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/hmac_sha256.h"
#include "umicom/base/sha256.h"

#include <stdint.h>
#include <string.h>

#define HMAC_BLOCK_BYTES 64U

/* Best-effort clearing of transient key material. Security deployments must
 * additionally use their approved key store and platform erasure facilities;
 * ordinary C memory erasure alone cannot guarantee forensic inaccessibility. */
static void HmacClear(void *memory, size_t size)
{
    volatile uint8_t *bytes = (volatile uint8_t *)memory;
    while (size-- != 0U) *bytes++ = 0U;
}

/* Output buffers may never overlap the secret or message. An overlap check
 * expressed with integer bounds is conservative on unusual pointer models. */
static int HmacOverlaps(const void *left, size_t leftSize,
                        const void *right, size_t rightSize)
{
    uintptr_t first = (uintptr_t)left, second = (uintptr_t)right;
    if (leftSize == 0U || rightSize == 0U) return 0;
    if (first > UINTPTR_MAX - leftSize ||
        second > UINTPTR_MAX - rightSize) return 1;
    return first < second + rightSize && second < first + leftSize;
}

UmiStatus UmiSecurityHmacSha256Compute(
    const uint8_t *key, size_t keyBytes,
    const uint8_t *data, size_t dataBytes,
    uint8_t outMac[UMI_SECURITY_HMAC_SHA256_BYTES])
{
    uint8_t block[HMAC_BLOCK_BYTES] = {0};
    uint8_t ipad[HMAC_BLOCK_BYTES] = {0};
    uint8_t opad[HMAC_BLOCK_BYTES] = {0};
    uint8_t inner[UMI_SECURITY_HMAC_SHA256_BYTES] = {0};
    uint8_t candidate[UMI_SECURITY_HMAC_SHA256_BYTES] = {0};
    UmiSha256 hash;
    UmiStatus status = UMI_STATUS_OK;
    if (key == NULL || keyBytes == 0U || outMac == NULL ||
        (data == NULL && dataBytes != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (HmacOverlaps(outMac, sizeof(candidate), key, keyBytes) ||
        HmacOverlaps(outMac, sizeof(candidate), data, dataBytes))
        return UMI_STATUS_INVALID_ARGUMENT;

    if (keyBytes > HMAC_BLOCK_BYTES) {
        UmiSha256Init(&hash);
        status = UmiSha256Update(&hash, key, keyBytes);
        if (status == UMI_STATUS_OK)
            status = UmiSha256Final(&hash, block);
    } else {
        memcpy(block, key, keyBytes);
    }
    if (status != UMI_STATUS_OK) goto finish;
    for (size_t index = 0U; index < HMAC_BLOCK_BYTES; ++index) {
        ipad[index] = (uint8_t)(block[index] ^ UINT8_C(0x36));
        opad[index] = (uint8_t)(block[index] ^ UINT8_C(0x5c));
    }
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, ipad, sizeof(ipad));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, data, dataBytes);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&hash, inner);
    if (status != UMI_STATUS_OK) goto finish;
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, opad, sizeof(opad));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, inner, sizeof(inner));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&hash, candidate);
    if (status == UMI_STATUS_OK)
        memcpy(outMac, candidate, sizeof(candidate));
finish:
    HmacClear(&hash, sizeof(hash));
    HmacClear(block, sizeof(block));
    HmacClear(ipad, sizeof(ipad));
    HmacClear(opad, sizeof(opad));
    HmacClear(inner, sizeof(inner));
    HmacClear(candidate, sizeof(candidate));
    return status;
}

UmiStatus UmiSecurityHmacSha256Verify(
    const uint8_t *key, size_t keyBytes,
    const uint8_t *data, size_t dataBytes,
    const uint8_t claimedMac[UMI_SECURITY_HMAC_SHA256_BYTES])
{
    uint8_t candidate[UMI_SECURITY_HMAC_SHA256_BYTES] = {0};
    unsigned difference = 0U;
    if (claimedMac == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiSecurityHmacSha256Compute(
        key, keyBytes, data, dataBytes, candidate);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < sizeof(candidate); ++index)
        difference |= (unsigned)(candidate[index] ^ claimedMac[index]);
    HmacClear(candidate, sizeof(candidate));
    return difference == 0U ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
