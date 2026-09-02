/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/trading_profile.c
 *
 * PURPOSE:
 *   Describe canonical Trader profile capability names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of trading profile. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/trading_profile.h"
static const char *caps[]={"umicom.finance","umicom.trading.market-data","umicom.trading.orders","umicom.trading.risk","umicom.trading.replay","umicom.trading.research","umicom.chart"};
/*
 * Return the number of records represented by trading profile capability without changing
 * their state.
 */
size_t umi_trading_profile_capability_count(void){return sizeof(caps)/sizeof(caps[0]);}
/*
 * Find trading profile capability while leaving the underlying catalogue or model owned by
 * this module.
 */
const char *umi_trading_profile_capability_at(size_t index){return index<umi_trading_profile_capability_count()?caps[index]:NULL;}
