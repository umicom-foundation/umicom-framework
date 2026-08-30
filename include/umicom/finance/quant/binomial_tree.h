/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/binomial_tree.h
 *
 * PURPOSE:
 *   Price bounded-step European options with a Cox-Ross-Rubinstein tree.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BINOMIAL_TREE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BINOMIAL_TREE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a European option with up to 256 CRR binomial steps. */
UmiStatus umi_quant_binomial_tree_price(double spot, double strike, double rate, double volatility, double years, uint32_t steps, UmiQuantOptionRight right, double *out_price);

#ifdef __cplusplus
}
#endif

#endif
