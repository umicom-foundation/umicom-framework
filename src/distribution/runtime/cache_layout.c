/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/cache_layout.c
 *
 * PURPOSE:
 *   cache namespace and eviction-budget configuration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/cache_layout.h"

void umi_dr_cache_layout_init(UmiDrCacheLayout *value) { if (value != NULL) { *value = (UmiDrCacheLayout){0};  } }
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value) { return value != NULL && (value->id[0] != '\0' && value->namespace_id[0] != '\0' && value->max_bytes > 0U); }
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_cache_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
