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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/submission_approval.h"

#include <string.h>

UmiStatus umi_reg_submission_approval_init(UmiSubmissionApproval *record, const char *package_id, const char *approver_id, int64_t approved_ms, int approved)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(approved_ms >= 0 && (approved == 0 || approved == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->package_id, sizeof record->package_id, package_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->approver_id, sizeof record->approver_id, approver_id);
    if (status != UMI_STATUS_OK) return status;
    record->approved_ms = approved_ms;
    record->approved = approved;
    return UMI_STATUS_OK;
}
