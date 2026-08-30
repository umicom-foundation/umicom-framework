/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/token.c
 *
 * PURPOSE:
 *   Implement deterministic non-secret fingerprints, constant-time equality and
 *   compiler-resistant zeroing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/token.h"

#include <stddef.h>
#include <stdio.h>
uint64_t umi_token_hash64(const void *data, size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    if (data == NULL && length != 0U) return 0U;
    for (index = 0U; index < length; ++index) { hash ^= bytes[index]; hash *= UINT64_C(1099511628211); }
    return hash;
}
UmiStatus umi_token_fingerprint(const void *data, size_t length, char *out_text, size_t capacity)
{
    int written;
    if ((data == NULL && length != 0U) || out_text == NULL || capacity < UMI_TOKEN_FINGERPRINT_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(out_text, capacity, "%016llx", (unsigned long long)umi_token_hash64(data, length));
    return written == 16 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}
int umi_token_constant_time_equal(const void *left, const void *right, size_t length)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    unsigned char difference = 0U; size_t index;
    if ((left == NULL || right == NULL) && length != 0U) return 0;
    for (index = 0U; index < length; ++index) difference |= (unsigned char)(a[index] ^ b[index]);
    return difference == 0U;
}
void umi_secure_zero(void *data, size_t length)
{
    volatile unsigned char *bytes = (volatile unsigned char *)data;
    size_t index;
    if (data == NULL) return;
    for (index = 0U; index < length; ++index) bytes[index] = 0U;
}
