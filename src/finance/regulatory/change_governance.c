/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/change_governance.c
 *
 * PURPOSE:
 *   Record approval and effective-date governance for reporting-rule changes.
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

#include "umicom/finance/regulatory/change_governance.h"

#include <string.h>

UmiStatus umi_reg_change_governance_init(UmiChangeGovernance *record, const char *change_id, const char *approver_id, uint64_t change_hash, int64_t effective_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(change_hash != 0U && effective_ms >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->change_id, sizeof record->change_id, change_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->approver_id, sizeof record->approver_id, approver_id);
    if (status != UMI_STATUS_OK) return status;
    record->change_hash = change_hash;
    record->effective_ms = effective_ms;
    return UMI_STATUS_OK;
}
