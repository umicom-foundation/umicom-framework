/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/session_calendar.c
 *
 * PURPOSE:
 *   Determine whether a weekday number represents a standard trading weekday.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of session calendar. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/session_calendar.h"
int umi_trading_weekday_open(unsigned weekday){return weekday>=1U&&weekday<=5U;}
