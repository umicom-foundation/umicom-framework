/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/kill_switch.h
 *
 * PURPOSE:
 *   Represent an explicit trading kill switch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
typedef struct UmiTradingKillSwitch { int engaged; char reason[UMI_TRADING_TEXT_CAPACITY]; } UmiTradingKillSwitch;
void umi_kill_switch_engage(UmiTradingKillSwitch *state, const char *reason);
void umi_kill_switch_reset(UmiTradingKillSwitch *state);
#ifdef __cplusplus
}
#endif
#endif
