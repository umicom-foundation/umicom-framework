/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/self_trade.h
 *
 * PURPOSE:
 *   Detect opposite-side orders for the same account and instrument.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates self trade behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_SELF_TRADE_H
#define INCLUDE_UMICOM_TRADING_SELF_TRADE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_self_trade_risk(const UmiOrderRequest *left, const UmiOrderRequest *right);
#ifdef __cplusplus
}
#endif
#endif
