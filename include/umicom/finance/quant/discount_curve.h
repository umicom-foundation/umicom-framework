/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/discount_curve.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_DISCOUNT_CURVE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_DISCOUNT_CURVE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Calculate a continuously compounded discount factor from an annual zero rate. */
UmiStatus umi_quant_discount_curve_factor(double zero_rate, int32_t tenor_days, double *out_factor);
/* Discount a future amount using a zero rate and tenor. */
UmiStatus umi_quant_discount_curve_present_value(double future_amount, double zero_rate, int32_t tenor_days, double *out_present_value);

#ifdef __cplusplus
}
#endif

#endif
