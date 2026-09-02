/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/environment.h
 *
 * PURPOSE:
 *   Control whether an execution-capable action is permitted in a trading environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates environment behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ENVIRONMENT_H
#define INCLUDE_UMICOM_TRADING_ENVIRONMENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the trading environment allows live execution operation used by this module and
 * its client applications.
 */
int umi_trading_environment_allows_live_execution(UmiTradingEnvironment environment, int explicitly_armed);
#ifdef __cplusplus
}
#endif
#endif
