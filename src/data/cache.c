/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/cache.c
 *
 * PURPOSE:
 *   Implement bounded LRU text caching with replacement, eviction, hit and miss statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiCacheEntry {
    char *key;
    char *value;
    uint64_t age;
    int used;
} UmiCacheEntry;

struct UmiCache {
    UmiCacheEntry *entries;
    size_t capacity;
    size_t count;
    uint64_t clock;
    UmiCacheStats stats;
};

/* Provide the duplicate operation used by this module and its client applications. */
static char *duplicate(const char *value)
{
    size_t length = strlen(value);
    char *copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy != NULL) (void)memcpy(copy, value, length + 1U);
    return copy;
}

/* Initialise cache from caller-provided values so later operations receive a known state. */
UmiStatus umi_cache_create(size_t capacity, UmiCache **out_cache)
{
    UmiCache *cache;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_cache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_cache = NULL;
    cache = (UmiCache *)calloc(1U, sizeof(*cache));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    cache->entries = (UmiCacheEntry *)calloc(capacity, sizeof(*cache->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache->entries == NULL) {
        free(cache);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    cache->capacity = capacity;
    cache->stats.capacity = capacity;
    *out_cache = cache;
    return UMI_STATUS_OK;
}

/* Release or reset state held by cache so the same storage can be reused safely. */
void umi_cache_destroy(UmiCache *cache)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL) return;
    umi_cache_clear(cache);
    free(cache->entries);
    free(cache);
}

/* Provide the find operation used by this module and its client applications. */
static size_t find(const UmiCache *cache, const char *key)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < cache->capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (cache->entries[index].used &&
            strcmp(cache->entries[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the cache put operation used by this module and its client applications. */
UmiStatus umi_cache_put(UmiCache *cache,
                        const char *key,
                        const char *value)
{
    size_t index;
    size_t target = SIZE_MAX;
    uint64_t oldest = UINT64_MAX;
    char *key_copy;
    char *value_copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || key == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find(cache, key);
    key_copy = duplicate(key);
    value_copy = duplicate(value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key_copy == NULL || value_copy == NULL) {
        free(key_copy);
        free(value_copy);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX) {
        target = index;
        free(cache->entries[target].key);
        free(cache->entries[target].value);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < cache->capacity; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!cache->entries[index].used) {
                target = index;
                break;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cache->entries[index].age < oldest) {
                oldest = cache->entries[index].age;
                target = index;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (cache->entries[target].used) {
            free(cache->entries[target].key);
            free(cache->entries[target].value);
            cache->stats.evictions++;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            cache->count++;
        }
    }
    cache->entries[target].key = key_copy;
    cache->entries[target].value = value_copy;
    cache->entries[target].age = ++cache->clock;
    cache->entries[target].used = 1;
    cache->stats.entries = cache->count;
    return UMI_STATUS_OK;
}

/* Provide the cache get operation used by this module and its client applications. */
UmiStatus umi_cache_get(UmiCache *cache,
                        const char *key,
                        char *out_value,
                        size_t value_capacity)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || key == NULL || out_value == NULL ||
        value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find(cache, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        cache->stats.misses++;
        return UMI_STATUS_NOT_FOUND;
    }
    cache->stats.hits++;
    cache->entries[index].age = ++cache->clock;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(cache->entries[index].value) + 1U > value_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(out_value,
                   value_capacity,
                   "%s",
                   cache->entries[index].value);
    return UMI_STATUS_OK;
}

/* Remove cache while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_cache_remove(UmiCache *cache, const char *key)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find(cache, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    free(cache->entries[index].key);
    free(cache->entries[index].value);
    (void)memset(&cache->entries[index], 0, sizeof(cache->entries[index]));
    cache->count--;
    cache->stats.entries = cache->count;
    return UMI_STATUS_OK;
}

/* Release or reset state held by cache so the same storage can be reused safely. */
void umi_cache_clear(UmiCache *cache)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < cache->capacity; ++index) {
        free(cache->entries[index].key);
        free(cache->entries[index].value);
        (void)memset(&cache->entries[index], 0, sizeof(cache->entries[index]));
    }
    cache->count = 0U;
    cache->stats.entries = 0U;
}

/* Provide the cache stats operation used by this module and its client applications. */
UmiCacheStats umi_cache_stats(const UmiCache *cache)
{
    UmiCacheStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    return cache != NULL ? cache->stats : stats;
}
