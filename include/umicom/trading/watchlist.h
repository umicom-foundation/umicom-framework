/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/watchlist.h
 *
 * PURPOSE:
 *   Maintain a bounded watchlist of canonical instruments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates watchlist behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_WATCHLIST_H
#define INCLUDE_UMICOM_TRADING_WATCHLIST_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWatchlist { UmiInstrument instruments[UMI_TRADING_MAX_WATCHLIST]; size_t count; } UmiWatchlist;
void umi_watchlist_init(UmiWatchlist *watchlist);
UmiStatus umi_watchlist_add(UmiWatchlist *watchlist, const UmiInstrument *instrument);
#ifdef __cplusplus
}
#endif
#endif
