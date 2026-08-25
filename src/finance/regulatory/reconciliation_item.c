/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reconciliation_item.c
 *
 * PURPOSE:
 *   Represent one pair of financial values selected for reconciliation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reconciliation_item.h"

#include <string.h>

UmiStatus umi_reg_reconciliation_item_init(UmiReconciliationItem *record, const char *item_id, double left_value, double right_value, double tolerance)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(left_value) && umi_reg_number_valid(right_value) && umi_reg_number_valid(tolerance) && tolerance >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->item_id, sizeof record->item_id, item_id);
    if (status != UMI_STATUS_OK) return status;
    record->left_value = left_value;
    record->right_value = right_value;
    record->tolerance = tolerance;
    return UMI_STATUS_OK;
}
