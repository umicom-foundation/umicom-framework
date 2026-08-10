/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/cache.c
 *
 * PURPOSE:
 *   Implement bounded LRU text caching with replacement, eviction, hit and miss statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static char *duplicate(const char *value)
{
    size_t length = strlen(value);
    char *copy = (char *)malloc(length + 1U);
    if (copy != NULL) (void)memcpy(copy, value, length + 1U);
    return copy;
}

UmiStatus umi_cache_create(size_t capacity, UmiCache **out_cache)
{
    UmiCache *cache;
    if (capacity == 0U || out_cache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_cache = NULL;
    cache = (UmiCache *)calloc(1U, sizeof(*cache));
    if (cache == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    cache->entries = (UmiCacheEntry *)calloc(capacity, sizeof(*cache->entries));
    if (cache->entries == NULL) {
        free(cache);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    cache->capacity = capacity;
    cache->stats.capacity = capacity;
    *out_cache = cache;
    return UMI_STATUS_OK;
}

void umi_cache_destroy(UmiCache *cache)
{
    if (cache == NULL) return;
    umi_cache_clear(cache);
    free(cache->entries);
    free(cache);
}

static size_t find(const UmiCache *cache, const char *key)
{
    size_t index;
    for (index = 0U; index < cache->capacity; ++index) {
        if (cache->entries[index].used &&
            strcmp(cache->entries[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_cache_put(UmiCache *cache,
                        const char *key,
                        const char *value)
{
    size_t index;
    size_t target = SIZE_MAX;
    uint64_t oldest = UINT64_MAX;
    char *key_copy;
    char *value_copy;
    if (cache == NULL || key == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find(cache, key);
    key_copy = duplicate(key);
    value_copy = duplicate(value);
    if (key_copy == NULL || value_copy == NULL) {
        free(key_copy);
        free(value_copy);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (index != SIZE_MAX) {
        target = index;
        free(cache->entries[target].key);
        free(cache->entries[target].value);
    } else {
        for (index = 0U; index < cache->capacity; ++index) {
            if (!cache->entries[index].used) {
                target = index;
                break;
            }
            if (cache->entries[index].age < oldest) {
                oldest = cache->entries[index].age;
                target = index;
            }
        }
        if (cache->entries[target].used) {
            free(cache->entries[target].key);
            free(cache->entries[target].value);
            cache->stats.evictions++;
        } else {
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

UmiStatus umi_cache_get(UmiCache *cache,
                        const char *key,
                        char *out_value,
                        size_t value_capacity)
{
    size_t index;
    if (cache == NULL || key == NULL || out_value == NULL ||
        value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find(cache, key);
    if (index == SIZE_MAX) {
        cache->stats.misses++;
        return UMI_STATUS_NOT_FOUND;
    }
    cache->stats.hits++;
    cache->entries[index].age = ++cache->clock;
    if (strlen(cache->entries[index].value) + 1U > value_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(out_value,
                   value_capacity,
                   "%s",
                   cache->entries[index].value);
    return UMI_STATUS_OK;
}

UmiStatus umi_cache_remove(UmiCache *cache, const char *key)
{
    size_t index;
    if (cache == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find(cache, key);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    free(cache->entries[index].key);
    free(cache->entries[index].value);
    (void)memset(&cache->entries[index], 0, sizeof(cache->entries[index]));
    cache->count--;
    cache->stats.entries = cache->count;
    return UMI_STATUS_OK;
}

void umi_cache_clear(UmiCache *cache)
{
    size_t index;
    if (cache == NULL) return;
    for (index = 0U; index < cache->capacity; ++index) {
        free(cache->entries[index].key);
        free(cache->entries[index].value);
        (void)memset(&cache->entries[index], 0, sizeof(cache->entries[index]));
    }
    cache->count = 0U;
    cache->stats.entries = 0U;
}

UmiCacheStats umi_cache_stats(const UmiCache *cache)
{
    UmiCacheStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    return cache != NULL ? cache->stats : stats;
}
