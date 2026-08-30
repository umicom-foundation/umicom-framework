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

UmiStatus umi_reg_control_exception_init(UmiControlException *record, const char *exception_id, const char *control_id, const char *reason, int approved)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(approved == 0 || approved == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->exception_id, sizeof record->exception_id, exception_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->control_id, sizeof record->control_id, control_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->reason, sizeof record->reason, reason);
    if (status != UMI_STATUS_OK) return status;
    record->approved = approved;
    return UMI_STATUS_OK;
}
