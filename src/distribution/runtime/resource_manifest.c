/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/resource_manifest.c
 *
 * PURPOSE:
 *   resource-pack identity, locale, scale and content metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_manifest.h"

/*
 * Initialise dr resource manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_resource_manifest_init(UmiDrResourceManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrResourceManifest){0}; value->scale_percent=100U; } }
/*
 * Check that dr resource manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_resource_manifest_valid(const UmiDrResourceManifest *value) { return value != NULL && (value->id[0] != '\0' && value->scale_percent>=50U && value->scale_percent<=400U && value->digest[0] != '\0'); }
/*
 * Provide the dr resource manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_resource_manifest_fingerprint(const UmiDrResourceManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_resource_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
