/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/release_manifest.c
 *
 * PURPOSE:
 *   release identity, channel, platform matrix and artifact summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_manifest.h"

void umi_dr_release_manifest_init(UmiDrReleaseManifest *value) { if (value != NULL) { *value = (UmiDrReleaseManifest){0};  } }
bool umi_dr_release_manifest_valid(const UmiDrReleaseManifest *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->artifact_count>0U && value->platform_count>0U); }
uint64_t umi_dr_release_manifest_fingerprint(const UmiDrReleaseManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_release_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
