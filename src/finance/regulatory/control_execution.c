/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/control_execution.c
 *
 * PURPOSE:
 *   Record one control execution and deterministic outcome.
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

#include "umicom/finance/regulatory/control_execution.h"

#include <string.h>

UmiStatus umi_reg_control_execution_init(UmiControlExecution *record, const char *execution_id, const char *control_id, int64_t executed_ms, int passed)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(executed_ms >= 0 && (passed == 0 || passed == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->execution_id, sizeof record->execution_id, execution_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->control_id, sizeof record->control_id, control_id);
    if (status != UMI_STATUS_OK) return status;
    record->executed_ms = executed_ms;
    record->passed = passed;
    return UMI_STATUS_OK;
}
