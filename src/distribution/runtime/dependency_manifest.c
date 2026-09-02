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

/*
 * Initialise dr dependency manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_dependency_manifest_init(UmiDrDependencyManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDependencyManifest){0};  } }
/*
 * Check that dr dependency manifest satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_dependency_manifest_valid(const UmiDrDependencyManifest *value) { return value != NULL && (value->id[0] != '\0' && value->package_id[0] != '\0'); }
/*
 * Provide the dr dependency manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_dependency_manifest_fingerprint(const UmiDrDependencyManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_dependency_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
