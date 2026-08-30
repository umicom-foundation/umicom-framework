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

/* Return undiscounted FX-forward value for a long base-currency position. */
UmiStatus umi_quant_fx_forward_pricer_value(double notional, double contracted_forward, double market_forward, double discount_factor, double *out_pv);

#ifdef __cplusplus
}
#endif

#endif
