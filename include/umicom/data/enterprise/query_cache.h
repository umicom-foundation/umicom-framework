/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_cache.h
 *
 * PURPOSE:
 *   Cache bounded query-plan fingerprints and hit evidence without caching application-owned result objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_CACHE_H
#define UMICOM_DATA_ENTERPRISE_QUERY_CACHE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data query cache entry data shared with callers of this public contract.
 */
typedef struct UmiDataQueryCacheEntry { char cache_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; uint64_t plan_fingerprint; uint64_t created_at; uint64_t hits; bool valid; } UmiDataQueryCacheEntry;
/**
 * Represent the data query cache data shared with callers of this public contract.
 */
typedef struct UmiDataQueryCache { UmiDataQueryCacheEntry items[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiDataQueryCache;
/* Reset query-plan cache metadata. */ void umi_data_query_cache_init(UmiDataQueryCache *cache);
/* Upsert fingerprint evidence and preserve hit counters. */ UmiStatus umi_data_query_cache_put(UmiDataQueryCache *cache,const char *cache_id,uint64_t fingerprint,uint64_t created_at);
/* Mark a cache metadata hit and return the fingerprint. */ UmiStatus umi_data_query_cache_hit(UmiDataQueryCache *cache,const char *cache_id,uint64_t *out_fingerprint);
/* Invalidate all cached plans after schema changes. */ void umi_data_query_cache_invalidate_all(UmiDataQueryCache *cache);

#ifdef __cplusplus
}
#endif
#endif
