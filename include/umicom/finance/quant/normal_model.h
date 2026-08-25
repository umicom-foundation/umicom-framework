/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/normal_model.h
 *
 * PURPOSE:
 *   Price European options with the Bachelier normal model.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_NORMAL_MODEL_H
#define INCLUDE_UMICOM_FINANCE_QUANT_NORMAL_MODEL_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a European option on a normally distributed forward. */
UmiStatus umi_quant_normal_model_price(double forward, double strike, double normal_volatility, double years, double discount_factor, UmiQuantOptionRight right, double *out_price);

#ifdef __cplusplus
}
#endif

#endif
