/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/trading_profile.c
 *
 * PURPOSE:
 *   Describe canonical Trader profile capability names.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of trading profile. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/trading_profile.h"
static const char *caps[]={"umicom.finance","umicom.trading.market-data","umicom.trading.orders","umicom.trading.risk","umicom.trading.replay","umicom.trading.research","umicom.chart"};
size_t umi_trading_profile_capability_count(void){return sizeof(caps)/sizeof(caps[0]);}
const char *umi_trading_profile_capability_at(size_t index){return index<umi_trading_profile_capability_count()?caps[index]:NULL;}
