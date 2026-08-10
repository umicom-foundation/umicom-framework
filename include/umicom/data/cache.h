/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/cache.h
 *
 * PURPOSE:
 *   Declare a bounded least-recently-used text cache for read models and repository acceleration without changing authoritative storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_CACHE_H
#define UMICOM_DATA_CACHE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCache UmiCache;

typedef struct UmiCacheStats {
    size_t capacity;
    size_t entries;
    uint64_t hits;
    uint64_t misses;
    uint64_t evictions;
} UmiCacheStats;

UmiStatus umi_cache_create(size_t capacity, UmiCache **out_cache);
void umi_cache_destroy(UmiCache *cache);
UmiStatus umi_cache_put(UmiCache *cache,
                        const char *key,
                        const char *value);
UmiStatus umi_cache_get(UmiCache *cache,
                        const char *key,
                        char *out_value,
                        size_t value_capacity);
UmiStatus umi_cache_remove(UmiCache *cache, const char *key);
void umi_cache_clear(UmiCache *cache);
UmiCacheStats umi_cache_stats(const UmiCache *cache);

#ifdef __cplusplus
}
#endif

#endif
