/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/credit_spread.h
 *
 * PURPOSE:
 *   Represent credit spread over a benchmark yield.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_SPREAD_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_SPREAD_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant credit spread data shared with callers of this public contract.
 */
typedef struct UmiQuantCreditSpread {
    double risky_yield;
    double benchmark_yield;
} UmiQuantCreditSpread;

/* Initialise and validate the credit spread contract. */
UmiStatus umi_quant_credit_spread_init(UmiQuantCreditSpread *record, double risky_yield, double benchmark_yield);

/* Return risky yield minus benchmark yield. */
double umi_quant_credit_spread_spread(const UmiQuantCreditSpread *record);

#ifdef __cplusplus
}
#endif

#endif
