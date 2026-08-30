/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/cross_field_validation.h
 *
 * PURPOSE:
 *   Evaluate deterministic cross-field equality and tolerance constraints.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CROSS_FIELD_VALIDATION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CROSS_FIELD_VALIDATION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCrossFieldValidation {
    char rule_id[UMI_REG_ID_CAPACITY];
    double tolerance;
} UmiCrossFieldValidation;
/* Initialise a non-negative tolerance rule. */
UmiStatus umi_reg_cross_field_validation_init(UmiCrossFieldValidation *rule, const char *rule_id, double tolerance);
/* Return one when two report facts agree within the configured tolerance. */
int umi_reg_cross_field_validation_passes(const UmiCrossFieldValidation *rule, double left, double right);

#ifdef __cplusplus
}
#endif

#endif
