/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/cache_layout.c
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
#include "umicom/distribution/runtime/cache_layout.h"

/*
 * Initialise dr cache layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_cache_layout_init(UmiDrCacheLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrCacheLayout){0};  } }
/* Check that dr cache layout satisfies its contract before another service relies on it. */
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value) { return value != NULL && (value->id[0] != '\0' && value->namespace_id[0] != '\0' && value->max_bytes > 0U); }
/*
 * Provide the dr cache layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_cache_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
