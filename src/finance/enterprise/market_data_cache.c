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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_cache.h"

#include <string.h>

/*
 * Initialise enterprise market data cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_market_data_cache_init(UmiEnterpriseMarketDataCache *cache)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache != NULL) memset(cache, 0, sizeof *cache);
}

/*
 * Find enterprise market data cache while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEnterpriseMarketDataUpdate *umi_enterprise_market_data_cache_find(const UmiEnterpriseMarketDataCache *cache, const char *topic_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || topic_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < cache->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(cache->updates[i].topic_id, topic_id) == 0) return &cache->updates[i];
    return NULL;
}

/*
 * Provide the enterprise market data cache put operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_market_data_cache_put(UmiEnterpriseMarketDataCache *cache, const UmiEnterpriseMarketDataUpdate *update)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || update == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < cache->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(cache->updates[i].topic_id, update->topic_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (update->sequence <= cache->updates[i].sequence) return UMI_STATUS_INVALID_STATE;
            cache->updates[i] = *update;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cache->count >= UMI_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    cache->updates[cache->count++] = *update;
    return UMI_STATUS_OK;
}
