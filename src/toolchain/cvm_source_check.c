/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_source_check.c
 *
 * PURPOSE:
 *   Reset request state and delegate bounded source diagnostics to Umicom C.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_source_check.h"
#include "cvm_status.h"

UmiStatus umi_cvm_check_source(const char *source, size_t length,
    uint64_t source_revision, UmiCvmSourceCheckResult *result, size_t result_size)
{
    if (result == NULL || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Framework owns request freshness. Clearing an admitted result before
     * delegation prevents a failed edit from inheriting prior warning rows. */
    *result = (UmiCvmSourceCheckResult){.source_revision = source_revision};
    const UmcStatus status = umc_check_source(source, length, &result->report,
        &result->diagnostic);
    if (status == UMC_OK) result->completed = true;
    return UmiCvmTranslateStatus(status);
}

bool umi_cvm_source_check_matches(const UmiCvmSourceCheckResult *result,
    size_t result_size, uint64_t current_source_revision)
{
    return result != NULL && result_size == sizeof *result && result->completed &&
        result->source_revision == current_source_revision;
}
