/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/trading_calendar.h
 *
 * PURPOSE:
 *   Define weekday trading availability for an exchange venue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TRADING_CALENDAR_H
#define UMICOM_TRADING_CORE_TRADING_CALENDAR_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingCalendar { bool weekday_open[7]; uint32_t open_days; } UmiTradingCalendar;
/* Initialise a Monday-Friday trading calendar. */
void umi_trading_calendar_weekdays(UmiTradingCalendar *calendar);
/* Set one weekday open or closed and maintain the day count. */
UmiStatus umi_trading_calendar_set(UmiTradingCalendar *calendar,size_t weekday,bool is_open);
/* Query whether the zero-based weekday is a trading day. */
bool umi_trading_calendar_is_open(const UmiTradingCalendar *calendar,size_t weekday);
#ifdef __cplusplus
}
#endif
#endif
