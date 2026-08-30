/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/momentum.h
 *
 * PURPOSE:
 *   Calculate simple close-to-close momentum.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates momentum behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_MOMENTUM_H
#define INCLUDE_UMICOM_TRADING_MOMENTUM_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_price_momentum(double previous_close, double current_close);
#ifdef __cplusplus
}
#endif
#endif
