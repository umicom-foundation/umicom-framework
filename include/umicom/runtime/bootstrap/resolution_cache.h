/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_cache.h
 *
 * PURPOSE:
 *   Cache resolved instances by qualified service key within an allowed scope.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CACHE_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CACHE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_resolution_cache_init(UmiBootstrapResolutionCache *cache);
UmiStatus umi_bootstrap_resolution_cache_put(UmiBootstrapResolutionCache *cache,
                                             const UmiBootstrapServiceKey *key,
                                             void *instance);
void *umi_bootstrap_resolution_cache_find(const UmiBootstrapResolutionCache *cache,
                                          const UmiBootstrapServiceKey *key);

#ifdef __cplusplus
}
#endif

#endif
