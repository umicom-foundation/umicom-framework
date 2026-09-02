/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/integrity.c
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

#include "umicom/delivery/integrity.h"
#include "delivery_internal.h"
#include <string.h>

/* Provide the integrity verify operation used by this module and its client applications. */
UmiStatus umi_integrity_verify(UmiIntegrityResult *result,
                               const char *artifact_id,
                               const char *expected,
                               const char *actual)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || artifact_id == NULL || expected == NULL || actual == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(result, 0, sizeof(*result));
    status = umi_delivery_copy_text(result->artifact_id,
                                    sizeof(result->artifact_id), artifact_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(result->expected, sizeof(result->expected), expected);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(result->actual, sizeof(result->actual), actual);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    result->status = strcmp(expected, actual) == 0 ? UMI_EVIDENCE_PASS : UMI_EVIDENCE_FAIL;
    return UMI_STATUS_OK;
}
