/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/control_remediation.c
 *
 * PURPOSE:
 *   Represent owned remediation for a failed control or exception.
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

#include "umicom/finance/regulatory/control_remediation.h"

#include <string.h>

/*
 * Initialise reg control remediation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_control_remediation_init(UmiControlRemediation *record, const char *remediation_id, const char *exception_id, const char *owner_id, int64_t due_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(due_ms >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->remediation_id, sizeof record->remediation_id, remediation_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->exception_id, sizeof record->exception_id, exception_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->owner_id, sizeof record->owner_id, owner_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->due_ms = due_ms;
    return UMI_STATUS_OK;
}
