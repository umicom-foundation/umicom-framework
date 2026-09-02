/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reconciliation_break.c
 *
 * PURPOSE:
 *   Capture a material unresolved break produced by reconciliation.
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

#include "umicom/finance/regulatory/reconciliation_break.h"

#include <string.h>

/*
 * Initialise reg reconciliation break from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_reconciliation_break_init(UmiReconciliationBreak *record, const char *break_id, const char *item_id, double difference, UmiRegSeverity severity)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(umi_reg_number_valid(difference))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->break_id, sizeof record->break_id, break_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->item_id, sizeof record->item_id, item_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->difference = difference;
    record->severity = severity;
    return UMI_STATUS_OK;
}
