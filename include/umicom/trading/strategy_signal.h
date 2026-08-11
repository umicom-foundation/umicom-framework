/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/strategy_signal.h
 *
 * PURPOSE:
 *   Validate research/strategy signals without granting execution authority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates strategy signal behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_STRATEGY_SIGNAL_H
#define INCLUDE_UMICOM_TRADING_STRATEGY_SIGNAL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_strategy_signal_valid(const UmiStrategySignal *signal, int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
