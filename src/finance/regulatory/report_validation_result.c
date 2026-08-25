/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_validation_result.c
 *
 * PURPOSE:
 *   Capture deterministic pass/fail evidence for one report validation rule.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/report_validation_result.h"

#include <string.h>
UmiStatus umi_reg_report_validation_result_evaluate(UmiReportValidationResult *result, const char *rule_id, double value, double minimum, double maximum, UmiRegSeverity severity)
{
    UmiStatus status;
    if (result == NULL || !umi_reg_number_valid(value) || !umi_reg_number_valid(minimum) || !umi_reg_number_valid(maximum) || minimum > maximum) return UMI_STATUS_INVALID_ARGUMENT;
    memset(result,0,sizeof *result);
    status=umi_reg_copy_text(result->rule_id,sizeof result->rule_id,rule_id);
    if(status!=UMI_STATUS_OK) return status;
    result->observed_value=value; result->severity=severity; result->passed=(value>=minimum && value<=maximum)?1:0;
    return UMI_STATUS_OK;
}
