/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/concentration_monitor.c
 *
 * PURPOSE:
 *   Measure participant share of venue volume against a configurable concentration limit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/concentration_monitor.h"

uint32_t umi_trading_concentration_monitor_bps(uint64_t participant_volume,uint64_t venue_volume){if(venue_volume==0U)return 0U;if(participant_volume>=venue_volume)return 10000U;uint64_t remainder=participant_volume;uint64_t accumulator=0U;uint32_t quotient=0U;for(uint32_t i=0U;i<10000U;i++){if(accumulator>=venue_volume-remainder){accumulator-=venue_volume-remainder;quotient++;}else{accumulator+=remainder;}}return quotient;}
/* Return whether participant concentration meets or exceeds a configured limit. */
bool umi_trading_concentration_monitor_breached(uint64_t participant_volume,uint64_t venue_volume,uint32_t limit_bps){return limit_bps<=10000U&&umi_trading_concentration_monitor_bps(participant_volume,venue_volume)>=limit_bps;}
