/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_session.c
 *
 * PURPOSE:
 *   Represent a simple daily market-session window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of market session. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_session.h"
int umi_market_session_contains(int64_t open_ms,int64_t close_ms,int64_t value_ms){return open_ms<=value_ms && value_ms<close_ms;}
