/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/control_exception.c
 *
 * PURPOSE:
 *   Represent an approved or open exception to a financial control.
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

#include "umicom/finance/regulatory/control_exception.h"

#include <string.h>

/*
 * Initialise reg control exception from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_reg_control_exception_init(UmiControlException *record, const char *exception_id, const char *control_id, const char *reason, int approved)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(approved == 0 || approved == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->exception_id, sizeof record->exception_id, exception_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->control_id, sizeof record->control_id, control_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->reason, sizeof record->reason, reason);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->approved = approved;
    return UMI_STATUS_OK;
}
