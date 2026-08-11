/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/price_band.h
 *
 * PURPOSE:
 *   Validate an order price against a reference-price percentage band.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates price band behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_PRICE_BAND_H
#define INCLUDE_UMICOM_TRADING_PRICE_BAND_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_price_within_band(double price, double reference_price, double maximum_fraction);
#ifdef __cplusplus
}
#endif
#endif
