/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_validation_rule.c
 *
 * PURPOSE:
 *   Define one numeric or presence rule for regulatory report validation.
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

#include "umicom/finance/regulatory/report_validation_rule.h"

#include <string.h>

/*
 * Initialise reg report validation rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_report_validation_rule_init(UmiReportValidationRule *record, const char *rule_id, const char *field_id, double minimum, double maximum)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(umi_reg_number_valid(minimum) && umi_reg_number_valid(maximum) && minimum <= maximum)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->rule_id, sizeof record->rule_id, rule_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->field_id, sizeof record->field_id, field_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->minimum = minimum;
    record->maximum = maximum;
    return UMI_STATUS_OK;
}
