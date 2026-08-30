/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/fingerprint_portable.c
 *
 * PURPOSE:
 *   Implement the portable FNV-1a 64-bit fingerprint backend and stable seed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/fingerprint.h"

#define UMI_DOCUMENT_FNV1A_PRIME UINT64_C(1099511628211)

uint64_t umi_document_hash_bytes_portable(const void *bytes,
                                          size_t byte_count,
                                          uint64_t initial_hash)
{
    const unsigned char *input = (const unsigned char *)bytes;
    size_t index;
    uint64_t hash = initial_hash;
    if (input == NULL && byte_count > 0U) return 0U;
    for (index = 0U; index < byte_count; ++index) {
        hash ^= input[index];
        hash *= UMI_DOCUMENT_FNV1A_PRIME;
    }
    return hash;
}
