/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/delta_package.c
 *
 * PURPOSE:
 *   delta package base/target version and savings validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/delta_package.h"

void umi_dr_delta_package_init(UmiDrDeltaPackage *value) { if (value != NULL) { *value = (UmiDrDeltaPackage){0};  } }
bool umi_dr_delta_package_valid(const UmiDrDeltaPackage *value) { return value != NULL && (value->id[0] != '\0' && umi_dr_version_compare(value->target_version,value->base_version)>0 && value->full_size>0U && value->delta_size>0U && value->delta_size<value->full_size && value->digest[0] != '\0'); }
uint64_t umi_dr_delta_package_fingerprint(const UmiDrDeltaPackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_delta_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
