/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_validation_result.h
 *
 * PURPOSE:
 *   Capture deterministic pass/fail evidence for one report validation rule.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_VALIDATION_RESULT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_VALIDATION_RESULT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the report validation result data shared with callers of this public contract.
 */
typedef struct UmiReportValidationResult {
    char rule_id[UMI_REG_ID_CAPACITY];
    int passed;
    UmiRegSeverity severity;
    double observed_value;
} UmiReportValidationResult;
/* Evaluate one observed value against an inclusive numeric interval. */
UmiStatus umi_reg_report_validation_result_evaluate(UmiReportValidationResult *result, const char *rule_id, double value, double minimum, double maximum, UmiRegSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
