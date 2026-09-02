/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/integrity.h
 *
 * PURPOSE:
 *   Compare expected and observed digests and retain machine-readable verification evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Integrity verification answers whether an artifact matches the bytes that the release metadata expected.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_INTEGRITY_H
#define INCLUDE_UMICOM_DELIVERY_INTEGRITY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the integrity result data shared with callers of this public contract.
 */
typedef struct UmiIntegrityResult {
    char artifact_id[UMI_DELIVERY_ID_CAPACITY];
    char expected[UMI_DELIVERY_DIGEST_CAPACITY];
    char actual[UMI_DELIVERY_DIGEST_CAPACITY];
    UmiEvidenceStatus status;
} UmiIntegrityResult;

/**
 * Provide the integrity verify operation used by this module and its client applications.
 */
UmiStatus umi_integrity_verify(UmiIntegrityResult *result,
                               const char *artifact_id,
                               const char *expected,
                               const char *actual);

#ifdef __cplusplus
}
#endif

#endif
