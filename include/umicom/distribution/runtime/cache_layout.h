/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/cache_layout.h
 *
 * PURPOSE:
 *   cache namespace and eviction-budget configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CACHE_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_CACHE_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr cache layout data shared with callers of this public contract.
 */
typedef struct UmiDrCacheLayout { char id[UMI_DR_ID_CAPACITY]; char namespace_id[UMI_DR_ID_CAPACITY]; uint64_t max_bytes; bool disposable; } UmiDrCacheLayout;
/**
 * Initialise dr cache layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_cache_layout_init(UmiDrCacheLayout *value);
/**
 * Check that dr cache layout satisfies its contract before another service relies on it.
 */
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value);
/**
 * Provide the dr cache layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value);

#ifdef __cplusplus
}
#endif
#endif
