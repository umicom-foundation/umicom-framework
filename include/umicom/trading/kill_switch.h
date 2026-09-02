/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/kill_switch.h
 *
 * PURPOSE:
 *   Represent an explicit trading kill switch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates kill switch behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_KILL_SWITCH_H
#define INCLUDE_UMICOM_TRADING_KILL_SWITCH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading kill switch data shared with callers of this public contract.
 */
typedef struct UmiTradingKillSwitch { int engaged; char reason[UMI_TRADING_TEXT_CAPACITY]; } UmiTradingKillSwitch;
/**
 * Provide the kill switch engage operation used by this module and its client
 * applications.
 */
void umi_kill_switch_engage(UmiTradingKillSwitch *state, const char *reason);
/**
 * Release or reset state held by kill switch so the same storage can be reused safely.
 */
void umi_kill_switch_reset(UmiTradingKillSwitch *state);
#ifdef __cplusplus
}
#endif
#endif
