/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_cache.c
 *
 * PURPOSE:
 *   Maintain the latest update per topic in a bounded deterministic cache.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_cache.h"

#include <string.h>

void umi_enterprise_market_data_cache_init(UmiEnterpriseMarketDataCache *cache)
{
    if (cache != NULL) memset(cache, 0, sizeof *cache);
}

const UmiEnterpriseMarketDataUpdate *umi_enterprise_market_data_cache_find(const UmiEnterpriseMarketDataCache *cache, const char *topic_id)
{
    size_t i;
    if (cache == NULL || topic_id == NULL) return NULL;
    for (i = 0U; i < cache->count; ++i) if (strcmp(cache->updates[i].topic_id, topic_id) == 0) return &cache->updates[i];
    return NULL;
}

UmiStatus umi_enterprise_market_data_cache_put(UmiEnterpriseMarketDataCache *cache, const UmiEnterpriseMarketDataUpdate *update)
{
    size_t i;
    if (cache == NULL || update == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < cache->count; ++i) {
        if (strcmp(cache->updates[i].topic_id, update->topic_id) == 0) {
            if (update->sequence <= cache->updates[i].sequence) return UMI_STATUS_INVALID_STATE;
            cache->updates[i] = *update;
            return UMI_STATUS_OK;
        }
    }
    if (cache->count >= UMI_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    cache->updates[cache->count++] = *update;
    return UMI_STATUS_OK;
}
