/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/market_data_snapshot.h
 *
 * PURPOSE:
 *   Store bounded scalar market-data observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_MARKET_DATA_SNAPSHOT_H
#define UMICOM_FINANCE_CORE_MARKET_DATA_SNAPSHOT_H

#include "umicom/finance/core/market_data_key.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiMarketDataObservation { UmiMarketDataKey key; double value; int64_t timestamp; } UmiMarketDataObservation;
typedef struct UmiMarketDataSnapshot { UmiFinancialId snapshot_id; UmiMarketDataObservation items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiMarketDataSnapshot;
/* Initialize snapshot. */ UmiStatus umi_market_data_snapshot_init(UmiMarketDataSnapshot *s,const char *id);
/* Add observation. */ UmiStatus umi_market_data_snapshot_add(UmiMarketDataSnapshot *s,const UmiMarketDataObservation *o);
/* Validate snapshot. */ bool umi_market_data_snapshot_is_valid(const UmiMarketDataSnapshot *s);
#ifdef __cplusplus
}
#endif

#endif
