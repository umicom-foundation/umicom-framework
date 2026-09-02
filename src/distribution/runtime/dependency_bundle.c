/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_bundle.c
 *
 * PURPOSE:
 *   resolved dependency set for one application release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_bundle.h"

/*
 * Initialise dr dependency bundle from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_dependency_bundle_init(UmiDrDependencyBundle *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDependencyBundle){0};  } }
/*
 * Check that dr dependency bundle satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_dependency_bundle_valid(const UmiDrDependencyBundle *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0'); }
/*
 * Provide the dr dependency bundle fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_dependency_bundle_fingerprint(const UmiDrDependencyBundle *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_dependency_bundle_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
