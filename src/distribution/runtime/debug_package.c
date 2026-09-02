/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/debug_package.c
 *
 * PURPOSE:
 *   diagnostic/debug companion package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/debug_package.h"

/*
 * Initialise dr debug package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_debug_package_init(UmiDrDebugPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDebugPackage){0};  } }
/* Check that dr debug package satisfies its contract before another service relies on it. */
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && (value->symbols || value->diagnostics || value->source_maps)); }
/*
 * Provide the dr debug package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_debug_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
