/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/curve_bootstrap.c
 *
 * PURPOSE:
 *   Bootstrap a simple zero-rate node from a par quote and prior discounting.
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

#include "umicom/finance/quant/curve_bootstrap.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_curve_bootstrap_discount_factor(double par_coupon, double period_fraction, double prior_coupon_pv, double *out_discount_factor)
{
    double denominator;
    if (out_discount_factor == NULL || par_coupon < 0.0 || period_fraction <= 0.0 || prior_coupon_pv < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    denominator = 1.0 + par_coupon * period_fraction;
    if (denominator <= 0.0) return UMI_STATUS_INVALID_STATE;
    *out_discount_factor = (1.0 - par_coupon * prior_coupon_pv) / denominator;
    return *out_discount_factor > 0.0 ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
