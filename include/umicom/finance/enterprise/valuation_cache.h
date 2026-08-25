/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_cache.h
 *
 * PURPOSE:
 *   Cache valuation outputs using deterministic product/market/model fingerprints.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CACHE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CACHE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_cache_key.h"
typedef struct UmiEnterpriseValuationCacheEntry { UmiEnterpriseValuationCacheKey key; double present_value; int64_t stored_ms; } UmiEnterpriseValuationCacheEntry;
typedef struct UmiEnterpriseValuationCache { UmiEnterpriseValuationCacheEntry entries[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiEnterpriseValuationCache;
/* Initialise an empty valuation result cache. */
void umi_enterprise_valuation_cache_init(UmiEnterpriseValuationCache *cache);
/* Insert or replace one finite valuation by full cache fingerprint. */
UmiStatus umi_enterprise_valuation_cache_put(UmiEnterpriseValuationCache *cache,const UmiEnterpriseValuationCacheEntry *entry);
/* Find a cached valuation by deterministic fingerprint. */
const UmiEnterpriseValuationCacheEntry *umi_enterprise_valuation_cache_find(const UmiEnterpriseValuationCache *cache,uint64_t fingerprint);

#ifdef __cplusplus
}
#endif

#endif
