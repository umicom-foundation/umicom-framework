/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/discount_curve.c
 *
 * PURPOSE:
 *   Convert zero rates into continuously compounded discount factors.
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

#include "umicom/finance/quant/discount_curve.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant discount curve factor operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_discount_curve_factor(double zero_rate, int32_t tenor_days, double *out_factor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_factor == NULL || tenor_days < 0 || !umi_quant_number_valid(zero_rate)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Retained for review: the previous direct assignment published overflow
     * as a successful price. Compute privately and commit only a finite value. */
#if 0
    *out_factor = exp(-zero_rate * ((double)tenor_days / 365.0));
#endif
    double exponent = -zero_rate * ((double)tenor_days / 365.0);
    if (!isfinite(exponent)) return UMI_STATUS_CAPACITY_EXCEEDED;
    double factor = exp(exponent);
    if (!isfinite(factor)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_factor = factor;
    return UMI_STATUS_OK;
}

/*
 * Provide the quant discount curve present value operation used by this module and its
 * client applications.
 */
UmiStatus umi_quant_discount_curve_present_value(double future_amount, double zero_rate, int32_t tenor_days, double *out_present_value)
{
    double factor; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_present_value == NULL || !umi_quant_number_valid(future_amount)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_quant_discount_curve_factor(zero_rate, tenor_days, &factor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the former assignment for comparison. The caller's last valid
     * result remains intact if multiplication exceeds the floating-point range. */
#if 0
    *out_present_value = future_amount * factor;
#endif
    double present_value = future_amount * factor;
    if (!isfinite(present_value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_present_value = present_value;
    return UMI_STATUS_OK;
}
