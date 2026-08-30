/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/concentration_monitor.h
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
#ifndef UMICOM_TRADING_CORE_CONCENTRATION_MONITOR_H
#define UMICOM_TRADING_CORE_CONCENTRATION_MONITOR_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Return participant concentration in basis points, clamped to 10000. */
uint32_t umi_trading_concentration_monitor_bps(uint64_t participant_volume,uint64_t venue_volume);
/* Return whether participant concentration meets or exceeds a limit. */
bool umi_trading_concentration_monitor_breached(uint64_t participant_volume,uint64_t venue_volume,uint32_t limit_bps);
#ifdef __cplusplus
}
#endif
#endif
