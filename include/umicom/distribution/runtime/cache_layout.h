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

typedef struct UmiDrCacheLayout { char id[UMI_DR_ID_CAPACITY]; char namespace_id[UMI_DR_ID_CAPACITY]; uint64_t max_bytes; bool disposable; } UmiDrCacheLayout;
void umi_dr_cache_layout_init(UmiDrCacheLayout *value);
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value);
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value);

#ifdef __cplusplus
}
#endif
#endif
