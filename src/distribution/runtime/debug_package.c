/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/debug_package.c
 *
 * PURPOSE:
 *   diagnostic/debug companion package metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/debug_package.h"

void umi_dr_debug_package_init(UmiDrDebugPackage *value) { if (value != NULL) { *value = (UmiDrDebugPackage){0};  } }
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && (value->symbols || value->diagnostics || value->source_maps)); }
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_debug_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
