/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reconciliation_rule.c
 *
 * PURPOSE:
 *   Define a tolerance and materiality threshold for financial reconciliation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reconciliation_rule.h"

#include <string.h>

UmiStatus umi_reg_reconciliation_rule_init(UmiReconciliationRule *record, const char *rule_id, const char *left_source, const char *right_source, double tolerance)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(umi_reg_number_valid(tolerance) && tolerance >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->rule_id, sizeof record->rule_id, rule_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->left_source, sizeof record->left_source, left_source);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->right_source, sizeof record->right_source, right_source);
    if (status != UMI_STATUS_OK) return status;
    record->tolerance = tolerance;
    return UMI_STATUS_OK;
}
