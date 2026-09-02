/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_manifest.c
 *
 * PURPOSE:
 *   cross-platform application identity and runtime requirement manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_manifest.h"

/*
 * Initialise dr application manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_manifest_init(UmiDrApplicationManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationManifest){0};  } }
/*
 * Check that dr application manifest satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_manifest_valid(const UmiDrApplicationManifest *value) { return value != NULL && (value->id[0] != '\0' && value->name[0] != '\0'); }
/*
 * Provide the dr application manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_manifest_fingerprint(const UmiDrApplicationManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_application_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
