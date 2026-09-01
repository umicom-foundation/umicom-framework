/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/buying_power.h
 *
 * PURPOSE:
 *   Calculate simple cash-based buying power from leverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates buying power behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_BUYING_POWER_H
#define INCLUDE_UMICOM_TRADING_BUYING_POWER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_buying_power(double cash_value, double leverage);
#ifdef __cplusplus
}
#endif
#endif
