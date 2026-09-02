/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/cache.h
 *
 * PURPOSE:
 *   Declare a bounded least-recently-used text cache for read models and repository acceleration without changing authoritative storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_CACHE_H
#define UMICOM_DATA_CACHE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the cache data shared with callers of this public contract.
 */
typedef struct UmiCache UmiCache;

/**
 * Represent the cache stats data shared with callers of this public contract.
 */
typedef struct UmiCacheStats {
    size_t capacity;
    size_t entries;
    uint64_t hits;
    uint64_t misses;
    uint64_t evictions;
} UmiCacheStats;

/**
 * Initialise cache from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_cache_create(size_t capacity, UmiCache **out_cache);
/**
 * Release or reset state held by cache so the same storage can be reused safely.
 */
void umi_cache_destroy(UmiCache *cache);
/**
 * Provide the cache put operation used by this module and its client applications.
 */
UmiStatus umi_cache_put(UmiCache *cache,
                        const char *key,
                        const char *value);
/**
 * Provide the cache get operation used by this module and its client applications.
 */
UmiStatus umi_cache_get(UmiCache *cache,
                        const char *key,
                        char *out_value,
                        size_t value_capacity);
/**
 * Remove cache while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_cache_remove(UmiCache *cache, const char *key);
/**
 * Release or reset state held by cache so the same storage can be reused safely.
 */
void umi_cache_clear(UmiCache *cache);
/**
 * Provide the cache stats operation used by this module and its client applications.
 */
UmiCacheStats umi_cache_stats(const UmiCache *cache);

#ifdef __cplusplus
}
#endif

#endif
