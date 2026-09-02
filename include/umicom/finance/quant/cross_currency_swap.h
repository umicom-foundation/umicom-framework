/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/cross_currency_swap.h
 *
 * PURPOSE:
 *   Represent cross-currency principal exchange economics.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CROSS_CURRENCY_SWAP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CROSS_CURRENCY_SWAP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant cross currency swap data shared with callers of this public
 * contract.
 */
typedef struct UmiQuantCrossCurrencySwap {
    double domestic_principal;
    double foreign_principal;
    double fx_rate;
} UmiQuantCrossCurrencySwap;

/* Initialise and validate the cross currency swap contract. */
UmiStatus umi_quant_cross_currency_swap_init(UmiQuantCrossCurrencySwap *record, double domestic_principal, double foreign_principal, double fx_rate);

/* Return domestic principal minus converted foreign principal. */
double umi_quant_cross_currency_swap_principal_mismatch(const UmiQuantCrossCurrencySwap *record);

#ifdef __cplusplus
}
#endif

#endif
