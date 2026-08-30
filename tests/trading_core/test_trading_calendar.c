/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_trading_calendar.c
 *
 * PURPOSE:
 *   Exercise define weekday trading availability for an exchange venue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trading_calendar.h"
int main(void) {

    UmiTradingCalendar c;
     umi_trading_calendar_weekdays(&c);
    if(c.open_days!=5U||!umi_trading_calendar_is_open(&c,1U)||umi_trading_calendar_is_open(&c,0U))return 1;
    if(umi_trading_calendar_set(&c,0U,true)!=UMI_STATUS_OK||c.open_days!=6U)return 2;
    return 0;
}
