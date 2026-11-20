/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/fx_forward_pricer.h
 *
 * PURPOSE:
 *   Price FX forwards from contracted and market forward rates.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FX_FORWARD_PRICER_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FX_FORWARD_PRICER_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return discounted quote-currency value for a long base-currency position.
 * Use a factor of 1.0 for the undiscounted observation. Rates must be positive,
 * while the notional and discount factor may be zero. Every input must be
 * finite. Refusal preserves *out_pv. An overflowing final value returns
 * CAPACITY_EXCEEDED. This floating-point value is for valuation review. */
UmiStatus umi_quant_fx_forward_pricer_value(double notional, double contracted_forward, double market_forward, double discount_factor, double *out_pv);

#ifdef __cplusplus
}
#endif

#endif
