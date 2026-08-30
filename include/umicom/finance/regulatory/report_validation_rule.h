/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_validation_rule.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_VALIDATION_RULE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_VALIDATION_RULE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportValidationRule {
    char rule_id[UMI_REG_ID_CAPACITY];
    char field_id[UMI_REG_ID_CAPACITY];
    double minimum;
    double maximum;
} UmiReportValidationRule;

/* Initialise one validated define one numeric or presence rule for regulatory report validation. record. */
UmiStatus umi_reg_report_validation_rule_init(UmiReportValidationRule *record, const char *rule_id, const char *field_id, double minimum, double maximum);

#ifdef __cplusplus
}
#endif

#endif
