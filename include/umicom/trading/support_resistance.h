/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/support_resistance.h
 *
 * PURPOSE:
 *   Measure room from current price to the nearest directional level.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates support resistance behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_SUPPORT_RESISTANCE_H
#define INCLUDE_UMICOM_TRADING_SUPPORT_RESISTANCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
double umi_room_to_level(double price, double level, int direction);
#ifdef __cplusplus
}
#endif
#endif
