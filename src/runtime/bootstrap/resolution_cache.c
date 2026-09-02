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
/*
 * Initialise bootstrap resolution cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_cache_init(UmiBootstrapResolutionCache *cache) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache != NULL) memset(cache, 0, sizeof(*cache));
}
/*
 * Find bootstrap resolution cache while leaving the underlying catalogue or model owned by
 * this module.
 */
void *umi_bootstrap_resolution_cache_find(const UmiBootstrapResolutionCache *cache,
                                          const UmiBootstrapServiceKey *key) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || key == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < cache->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_bootstrap_service_key_equal(&cache->items[i].key, key))
            return cache->items[i].instance;
    return NULL;
}
/*
 * Provide the bootstrap resolution cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_resolution_cache_put(UmiBootstrapResolutionCache *cache,
                                             const UmiBootstrapServiceKey *key,
                                             void *instance) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cache == NULL || key == NULL || instance == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < cache->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_bootstrap_service_key_equal(&cache->items[i].key, key)) {
            cache->items[i].instance = instance;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cache->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    cache->items[cache->count].key = *key;
    cache->items[cache->count].instance = instance;
    ++cache->count;
    return UMI_STATUS_OK;
}
