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

/*
 * This checksum provides a portable built-in integrity primitive; release signing remains a separate provider boundary.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_CHECKSUM_H
#define INCLUDE_UMICOM_DELIVERY_CHECKSUM_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named checksum algorithm values accepted by this public contract.
 */
typedef enum UmiChecksumAlgorithm {
    UMI_CHECKSUM_FNV1A64 = 1
} UmiChecksumAlgorithm;

/**
 * Represent the checksum record data shared with callers of this public contract.
 */
typedef struct UmiChecksumRecord {
    UmiChecksumAlgorithm algorithm;
    char path[UMI_DELIVERY_PATH_CAPACITY];
    char digest[UMI_DELIVERY_DIGEST_CAPACITY];
} UmiChecksumRecord;

/**
 * Provide the checksum fnv1a64 operation used by this module and its client applications.
 */
uint64_t umi_checksum_fnv1a64(const void *data, size_t length);
/**
 * Provide the checksum text operation used by this module and its client applications.
 */
UmiStatus umi_checksum_text(const char *text,
                            char *out_digest,
                            size_t digest_capacity);
/**
 * Initialise checksum record from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_checksum_record_init(UmiChecksumRecord *record,
                                   const char *path,
                                   const char *digest);

#ifdef __cplusplus
}
#endif

#endif
