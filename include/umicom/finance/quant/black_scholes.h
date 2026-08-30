/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/black_scholes.h
 *
 * PURPOSE:
 *   Price European equity-style options with continuous rates and dividends.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BLACK_SCHOLES_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BLACK_SCHOLES_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a European vanilla option under Black-Scholes-Merton assumptions. */
UmiStatus umi_quant_black_scholes_price(double spot, double strike, double risk_free_rate, double dividend_yield, double volatility, double years, UmiQuantOptionRight right, double *out_price);

#ifdef __cplusplus
}
#endif

#endif
