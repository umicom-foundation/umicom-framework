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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/discount_curve.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_discount_curve_factor(double zero_rate, int32_t tenor_days, double *out_factor)
{
    if (out_factor == NULL || tenor_days < 0 || !umi_quant_number_valid(zero_rate)) return UMI_STATUS_INVALID_ARGUMENT;
    *out_factor = exp(-zero_rate * ((double)tenor_days / 365.0));
    return UMI_STATUS_OK;
}

UmiStatus umi_quant_discount_curve_present_value(double future_amount, double zero_rate, int32_t tenor_days, double *out_present_value)
{
    double factor; UmiStatus status;
    if (out_present_value == NULL || !umi_quant_number_valid(future_amount)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_quant_discount_curve_factor(zero_rate, tenor_days, &factor);
    if (status != UMI_STATUS_OK) return status;
    *out_present_value = future_amount * factor;
    return UMI_STATUS_OK;
}
