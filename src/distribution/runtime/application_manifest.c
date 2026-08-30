/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_manifest.c
 *
 * PURPOSE:
 *   cross-platform application identity and runtime requirement manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_manifest.h"

void umi_dr_application_manifest_init(UmiDrApplicationManifest *value) { if (value != NULL) { *value = (UmiDrApplicationManifest){0};  } }
bool umi_dr_application_manifest_valid(const UmiDrApplicationManifest *value) { return value != NULL && (value->id[0] != '\0' && value->name[0] != '\0'); }
uint64_t umi_dr_application_manifest_fingerprint(const UmiDrApplicationManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_application_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
