/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/volatility_proxy.h
 *
 * PURPOSE:
 *   Calculate a simple bar-range volatility proxy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates volatility proxy behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_VOLATILITY_PROXY_H
#define INCLUDE_UMICOM_TRADING_VOLATILITY_PROXY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the bar volatility proxy operation used by this module and its client
 * applications.
 */
double umi_bar_volatility_proxy(const UmiBar *bar);
#ifdef __cplusplus
}
#endif
#endif
