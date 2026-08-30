/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_manifest.c
 *
 * PURPOSE:
 *   package dependency declaration with version and optionality constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_manifest.h"

void umi_dr_dependency_manifest_init(UmiDrDependencyManifest *value) { if (value != NULL) { *value = (UmiDrDependencyManifest){0};  } }
bool umi_dr_dependency_manifest_valid(const UmiDrDependencyManifest *value) { return value != NULL && (value->id[0] != '\0' && value->package_id[0] != '\0'); }
uint64_t umi_dr_dependency_manifest_fingerprint(const UmiDrDependencyManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_dependency_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
