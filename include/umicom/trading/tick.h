/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/tick.h
 *
 * PURPOSE:
 *   Validate normalised market trade ticks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates tick behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_TICK_H
#define INCLUDE_UMICOM_TRADING_TICK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_trade_tick_valid(const UmiTradeTick *tick);
#ifdef __cplusplus
}
#endif
#endif
