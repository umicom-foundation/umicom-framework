/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_manifest.c
 *
 * PURPOSE:
 *   published update metadata, version, channel and package fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_manifest.h"

void umi_dr_update_manifest_init(UmiDrUpdateManifest *value) { if (value != NULL) { *value = (UmiDrUpdateManifest){0};  } }
bool umi_dr_update_manifest_valid(const UmiDrUpdateManifest *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->package_digest[0] != '\0'); }
uint64_t umi_dr_update_manifest_fingerprint(const UmiDrUpdateManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_update_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
