/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_bundle.c
 *
 * PURPOSE:
 *   resolved dependency set for one application release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_bundle.h"

void umi_dr_dependency_bundle_init(UmiDrDependencyBundle *value) { if (value != NULL) { *value = (UmiDrDependencyBundle){0};  } }
bool umi_dr_dependency_bundle_valid(const UmiDrDependencyBundle *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0'); }
uint64_t umi_dr_dependency_bundle_fingerprint(const UmiDrDependencyBundle *value) {
    uint64_t h = 0U;
    if (!umi_dr_dependency_bundle_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
