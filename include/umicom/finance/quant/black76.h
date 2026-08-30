/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/black76.h
 *
 * PURPOSE:
 *   Price European options on forwards using the Black 1976 model.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BLACK76_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BLACK76_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a European option on a forward using Black 1976. */
UmiStatus umi_quant_black76_price(double forward, double strike, double volatility, double years, double discount_factor, UmiQuantOptionRight right, double *out_price);

#ifdef __cplusplus
}
#endif

#endif
