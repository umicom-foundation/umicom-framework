/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_approval.c
 *
 * PURPOSE:
 *   Record maker-checker approval for one submission package.
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

#include "umicom/finance/regulatory/submission_approval.h"

#include <string.h>

/*
 * Initialise reg submission approval from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_submission_approval_init(UmiSubmissionApproval *record, const char *package_id, const char *approver_id, int64_t approved_ms, int approved)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(approved_ms >= 0 && (approved == 0 || approved == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->package_id, sizeof record->package_id, package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->approver_id, sizeof record->approver_id, approver_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->approved_ms = approved_ms;
    record->approved = approved;
    return UMI_STATUS_OK;
}
