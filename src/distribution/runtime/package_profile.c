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

/*
 * Initialise dr package profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_package_profile_init(UmiDrPackageProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrPackageProfile){0}; value->compression_level=6U; } }
/*
 * Check that dr package profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value) { return value != NULL && (value->id[0] != '\0' && value->format != 0 && value->scope != 0 && value->compression_level<=9U); }
/*
 * Provide the dr package profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_package_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
