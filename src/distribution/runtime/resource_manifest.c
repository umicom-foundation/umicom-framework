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

void umi_dr_resource_manifest_init(UmiDrResourceManifest *value) { if (value != NULL) { *value = (UmiDrResourceManifest){0}; value->scale_percent=100U; } }
bool umi_dr_resource_manifest_valid(const UmiDrResourceManifest *value) { return value != NULL && (value->id[0] != '\0' && value->scale_percent>=50U && value->scale_percent<=400U && value->digest[0] != '\0'); }
uint64_t umi_dr_resource_manifest_fingerprint(const UmiDrResourceManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_resource_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
