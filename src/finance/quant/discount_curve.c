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
    *out_factor = exp(-zero_rate * ((double)tenor_days / 365.0));
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
    *out_present_value = future_amount * factor;
    return UMI_STATUS_OK;
}
