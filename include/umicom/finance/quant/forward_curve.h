/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/forward_curve.h
 *
 * PURPOSE:
 *   Derive simple-compounded forward rates between zero-curve tenors.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FORWARD_CURVE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FORWARD_CURVE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Derive a forward rate from two zero rates and maturities. */
UmiStatus umi_quant_forward_curve_rate(double near_zero, int32_t near_days, double far_zero, int32_t far_days, double *out_forward);

#ifdef __cplusplus
}
#endif

#endif
