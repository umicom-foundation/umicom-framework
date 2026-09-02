/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_package.c
 *
 * PURPOSE:
 *   Describe an immutable regulatory submission package fingerprint.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/submission_package.h"

#include <string.h>

/*
 * Initialise reg submission package from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_submission_package_init(UmiSubmissionPackage *record, const char *package_id, const char *report_id, const char *format, uint64_t fingerprint)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->package_id, sizeof record->package_id, package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->report_id, sizeof record->report_id, report_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->format, sizeof record->format, format);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->fingerprint = fingerprint;
    return UMI_STATUS_OK;
}
