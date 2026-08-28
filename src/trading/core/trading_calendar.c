/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/trading_calendar.c
 *
 * PURPOSE:
 *   Define weekday trading availability for an exchange venue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trading_calendar.h"

#include <string.h>
/* Initialise a Monday-Friday trading calendar. */
void umi_trading_calendar_weekdays(UmiTradingCalendar *calendar){if(calendar==NULL)return;memset(calendar,0,sizeof *calendar);for(size_t i=1;i<=5;i++)calendar->weekday_open[i]=true;calendar->open_days=5U;}
/* Set one weekday open or closed and maintain the day count. */
UmiStatus umi_trading_calendar_set(UmiTradingCalendar *calendar,size_t weekday,bool is_open){if(calendar==NULL||weekday>=7U)return UMI_STATUS_INVALID_ARGUMENT;if(calendar->weekday_open[weekday]!=is_open){calendar->weekday_open[weekday]=is_open;if(is_open)calendar->open_days++;else calendar->open_days--;}return UMI_STATUS_OK;}
/* Query whether the zero-based weekday is a trading day. */
bool umi_trading_calendar_is_open(const UmiTradingCalendar *calendar,size_t weekday){return calendar!=NULL&&weekday<7U&&calendar->weekday_open[weekday];}
