/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_cache.h
 *
 * PURPOSE:
 *   Cache resolved instances by qualified service key within an allowed scope.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CACHE_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CACHE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap resolution cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_cache_init(UmiBootstrapResolutionCache *cache);
/**
 * Provide the bootstrap resolution cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_resolution_cache_put(UmiBootstrapResolutionCache *cache,
                                             const UmiBootstrapServiceKey *key,
                                             void *instance);
/**
 * Find bootstrap resolution cache while leaving the underlying catalogue or model owned by
 * this module.
 */
void *umi_bootstrap_resolution_cache_find(const UmiBootstrapResolutionCache *cache,
                                          const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
