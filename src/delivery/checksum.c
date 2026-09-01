/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/checksum.c
 *
 * PURPOSE:
 *   Compute deterministic integrity digests for in-memory bytes and text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This checksum provides a portable built-in integrity primitive; release signing remains a separate provider boundary.
 */

#include "umicom/delivery/checksum.h"
#include "delivery_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

uint64_t umi_checksum_fnv1a64(const void *data, size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t hash = UINT64_C(14695981039346656037);
    size_t i;
    if (data == NULL && length != 0U) return 0U;
    for (i = 0U; i < length; ++i) {
        hash ^= (uint64_t)bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

UmiStatus umi_checksum_text(const char *text,
                            char *out_digest,
                            size_t digest_capacity)
{
    int written;
    uint64_t digest;
    if (text == NULL || out_digest == NULL || digest_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    digest = umi_checksum_fnv1a64(text, strlen(text));
    written = snprintf(out_digest, digest_capacity, "%016" PRIx64, digest);
    if (written < 0 || (size_t)written >= digest_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_checksum_record_init(UmiChecksumRecord *record,
                                   const char *path,
                                   const char *digest)
{
    UmiStatus status;
    if (record == NULL || path == NULL || digest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(record, 0, sizeof(*record));
    record->algorithm = UMI_CHECKSUM_FNV1A64;
    status = umi_delivery_copy_text(record->path, sizeof(record->path), path);
    if (status != UMI_STATUS_OK) return status;
    return umi_delivery_copy_text(record->digest, sizeof(record->digest), digest);
}
