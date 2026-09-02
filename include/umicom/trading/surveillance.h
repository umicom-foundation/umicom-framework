/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/surveillance.h
 *
 * PURPOSE:
 *   Flag unusually large orders relative to a configured threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates surveillance behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_SURVEILLANCE_H
#define INCLUDE_UMICOM_TRADING_SURVEILLANCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the surveillance large order operation used by this module and its client
 * applications.
 */
int umi_surveillance_large_order(const UmiOrderRequest *request, double threshold_quantity);
#ifdef __cplusplus
}
#endif
#endif
