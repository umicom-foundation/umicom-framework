/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_profile.c
 *
 * PURPOSE:
 *   named package profile selecting format, scope, compression and symbols policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_profile.h"

void umi_dr_package_profile_init(UmiDrPackageProfile *value) { if (value != NULL) { *value = (UmiDrPackageProfile){0}; value->compression_level=6U; } }
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value) { return value != NULL && (value->id[0] != '\0' && value->format != 0 && value->scope != 0 && value->compression_level<=9U); }
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value) {
    uint64_t h = 0U;
    if (!umi_dr_package_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
