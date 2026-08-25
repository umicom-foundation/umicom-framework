/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reconciliation_rule.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_RULE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_RULE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReconciliationRule {
    char rule_id[UMI_REG_ID_CAPACITY];
    char left_source[UMI_REG_ID_CAPACITY];
    char right_source[UMI_REG_ID_CAPACITY];
    double tolerance;
} UmiReconciliationRule;

/* Initialise one validated define a tolerance and materiality threshold for financial reconciliation. record. */
UmiStatus umi_reg_reconciliation_rule_init(UmiReconciliationRule *record, const char *rule_id, const char *left_source, const char *right_source, double tolerance);

#ifdef __cplusplus
}
#endif

#endif
