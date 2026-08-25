/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/curve_bootstrap.h
 *
 * PURPOSE:
 *   Bootstrap a simple zero-rate node from a par quote and prior discounting.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CURVE_BOOTSTRAP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CURVE_BOOTSTRAP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bootstrap a terminal discount factor from par coupon and prior coupon PV. */
UmiStatus umi_quant_curve_bootstrap_discount_factor(double par_coupon, double period_fraction, double prior_coupon_pv, double *out_discount_factor);

#ifdef __cplusplus
}
#endif

#endif
