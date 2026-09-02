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

/*
 * Initialise dr delta package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_delta_package_init(UmiDrDeltaPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDeltaPackage){0};  } }
/* Check that dr delta package satisfies its contract before another service relies on it. */
bool umi_dr_delta_package_valid(const UmiDrDeltaPackage *value) { return value != NULL && (value->id[0] != '\0' && umi_dr_version_compare(value->target_version,value->base_version)>0 && value->full_size>0U && value->delta_size>0U && value->delta_size<value->full_size && value->digest[0] != '\0'); }
/*
 * Provide the dr delta package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_delta_package_fingerprint(const UmiDrDeltaPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_delta_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
