/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/capital_measure.h
 *
 * PURPOSE:
 *   Represent a regulatory capital numerator or exposure measure.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CAPITAL_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CAPITAL_MEASURE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the capital measure data shared with callers of this public contract.
 */
typedef struct UmiCapitalMeasure {
    char measure_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    double amount;
    UmiCurrency currency;
} UmiCapitalMeasure;

/* Initialise one validated represent a regulatory capital numerator or exposure measure. record. */
UmiStatus umi_reg_capital_measure_init(UmiCapitalMeasure *record, const char *measure_id, const char *name, double amount, UmiCurrency currency);

#ifdef __cplusplus
}
#endif

#endif
