/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/checksum.h
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

/* BEGINNER NOTE:
 * This checksum provides a portable built-in integrity primitive; release signing remains a separate provider boundary.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_CHECKSUM_H
#define INCLUDE_UMICOM_DELIVERY_CHECKSUM_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiChecksumAlgorithm {
    UMI_CHECKSUM_FNV1A64 = 1
} UmiChecksumAlgorithm;

typedef struct UmiChecksumRecord {
    UmiChecksumAlgorithm algorithm;
    char path[UMI_DELIVERY_PATH_CAPACITY];
    char digest[UMI_DELIVERY_DIGEST_CAPACITY];
} UmiChecksumRecord;

uint64_t umi_checksum_fnv1a64(const void *data, size_t length);
UmiStatus umi_checksum_text(const char *text,
                            char *out_digest,
                            size_t digest_capacity);
UmiStatus umi_checksum_record_init(UmiChecksumRecord *record,
                                   const char *path,
                                   const char *digest);

#ifdef __cplusplus
}
#endif

#endif
