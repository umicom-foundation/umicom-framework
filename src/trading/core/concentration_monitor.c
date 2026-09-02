/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/concentration_monitor.c
 *
 * PURPOSE:
 *   Measure participant share of venue volume against a configurable concentration limit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/concentration_monitor.h"

/*
 * Provide the trading concentration monitor bps operation used by this module and its
 * client applications.
 */
uint32_t umi_trading_concentration_monitor_bps(uint64_t participant_volume,uint64_t venue_volume){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(venue_volume==0U)return 0U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(participant_volume>=venue_volume)return 10000U;uint64_t remainder=participant_volume;uint64_t accumulator=0U;uint32_t quotient=0U;/* Visit each bounded item once so every record receives the same rule. */ for(uint32_t i=0U;i<10000U;i++){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(accumulator>=venue_volume-remainder){accumulator-=venue_volume-remainder;quotient++;}/* Use this fallback path when the earlier condition does not apply. */ else{accumulator+=remainder;}}return quotient;}
/* Return whether participant concentration meets or exceeds a configured limit. */
bool umi_trading_concentration_monitor_breached(uint64_t participant_volume,uint64_t venue_volume,uint32_t limit_bps){return limit_bps<=10000U&&umi_trading_concentration_monitor_bps(participant_volume,venue_volume)>=limit_bps;}
