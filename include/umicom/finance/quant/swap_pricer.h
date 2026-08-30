/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/swap_pricer.h
 *
 * PURPOSE:
 *   Price fixed-versus-floating swaps from a par-rate difference and annuity.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SWAP_PRICER_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SWAP_PRICER_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return receive-floating/pay-fixed swap PV from notional, annuity and rate difference. */
UmiStatus umi_quant_swap_pricer_value(double notional, double annuity, double fixed_rate, double par_rate, double *out_pv);

#ifdef __cplusplus
}
#endif

#endif
