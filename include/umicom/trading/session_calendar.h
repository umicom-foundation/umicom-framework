/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/session_calendar.h
 *
 * PURPOSE:
 *   Determine whether a weekday number represents a standard trading weekday.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates session calendar behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_SESSION_CALENDAR_H
#define INCLUDE_UMICOM_TRADING_SESSION_CALENDAR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_trading_weekday_open(unsigned weekday);
#ifdef __cplusplus
}
#endif
#endif
