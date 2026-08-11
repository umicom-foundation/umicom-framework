/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/integrity.c
 *
 * PURPOSE:
 *   Compare expected and observed digests and retain machine-readable verification evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Integrity verification answers whether an artifact matches the bytes that the release metadata expected.
 */

#include "umicom/delivery/integrity.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_integrity_verify(UmiIntegrityResult *result,
                               const char *artifact_id,
                               const char *expected,
                               const char *actual)
{
    UmiStatus status;
    if (result == NULL || artifact_id == NULL || expected == NULL || actual == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(result, 0, sizeof(*result));
    status = umi_delivery_copy_text(result->artifact_id,
                                    sizeof(result->artifact_id), artifact_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(result->expected, sizeof(result->expected), expected);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(result->actual, sizeof(result->actual), actual);
    if (status != UMI_STATUS_OK) return status;
    result->status = strcmp(expected, actual) == 0 ? UMI_EVIDENCE_PASS : UMI_EVIDENCE_FAIL;
    return UMI_STATUS_OK;
}
