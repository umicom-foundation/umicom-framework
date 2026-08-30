/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_cache.c
 *
 * PURPOSE:
 *   Implement the resolution cache behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolution_cache.c
 *
 * PURPOSE:
 *   Cache resolved instances by qualified service key within an allowed scope.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolution_cache.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>
void umi_bootstrap_resolution_cache_init(UmiBootstrapResolutionCache *cache) {
    if (cache != NULL) memset(cache, 0, sizeof(*cache));
}
void *umi_bootstrap_resolution_cache_find(const UmiBootstrapResolutionCache *cache,
                                          const UmiBootstrapServiceKey *key) {
    size_t i;
    if (cache == NULL || key == NULL) return NULL;
    for (i = 0U; i < cache->count; ++i)
        if (umi_bootstrap_service_key_equal(&cache->items[i].key, key))
            return cache->items[i].instance;
    return NULL;
}
UmiStatus umi_bootstrap_resolution_cache_put(UmiBootstrapResolutionCache *cache,
                                             const UmiBootstrapServiceKey *key,
                                             void *instance) {
    size_t i;
    if (cache == NULL || key == NULL || instance == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < cache->count; ++i) {
        if (umi_bootstrap_service_key_equal(&cache->items[i].key, key)) {
            cache->items[i].instance = instance;
            return UMI_STATUS_OK;
        }
    }
    if (cache->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    cache->items[cache->count].key = *key;
    cache->items[cache->count].instance = instance;
    ++cache->count;
    return UMI_STATUS_OK;
}
