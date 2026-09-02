/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_cache.h
 *
 * PURPOSE:
 *   Maintain the latest update per topic in a bounded deterministic cache.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_CACHE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_CACHE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/market_data_update.h"

/**
 * Represent the enterprise market data cache data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseMarketDataCache {
    UmiEnterpriseMarketDataUpdate updates[UMI_ENTERPRISE_MAX_ITEMS];
    size_t count;
} UmiEnterpriseMarketDataCache;

/* Initialise an empty latest-value cache. */
void umi_enterprise_market_data_cache_init(UmiEnterpriseMarketDataCache *cache);
/* Insert or replace a topic only when the sequence advances. */
UmiStatus umi_enterprise_market_data_cache_put(UmiEnterpriseMarketDataCache *cache, const UmiEnterpriseMarketDataUpdate *update);
/* Look up the latest update for a canonical topic. */
const UmiEnterpriseMarketDataUpdate *umi_enterprise_market_data_cache_find(const UmiEnterpriseMarketDataCache *cache, const char *topic_id);

#ifdef __cplusplus
}
#endif

#endif
