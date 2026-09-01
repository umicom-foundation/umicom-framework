/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/mean_reversion.h
 *
 * PURPOSE:
 *   Calculate signed distance from a reference mean.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates mean reversion behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MEAN_REVERSION_H
#define INCLUDE_UMICOM_TRADING_MEAN_REVERSION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_mean_reversion_distance(double price, double reference);
#ifdef __cplusplus
}
#endif
#endif
