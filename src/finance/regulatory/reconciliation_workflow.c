/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reconciliation_workflow.c
 *
 * PURPOSE:
 *   Track ownership and remediation state for reconciliation breaks.
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

#include "umicom/finance/regulatory/reconciliation_workflow.h"

#include <string.h>

UmiStatus umi_reg_reconciliation_workflow_init(UmiReconciliationWorkflow *record, const char *workflow_id, const char *break_id, const char *owner_id, int resolved)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(resolved == 0 || resolved == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->workflow_id, sizeof record->workflow_id, workflow_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->break_id, sizeof record->break_id, break_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->owner_id, sizeof record->owner_id, owner_id);
    if (status != UMI_STATUS_OK) return status;
    record->resolved = resolved;
    return UMI_STATUS_OK;
}
