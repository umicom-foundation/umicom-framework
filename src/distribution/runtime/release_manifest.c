/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/release_manifest.c
 *
 * PURPOSE:
 *   release identity, channel, platform matrix and artifact summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_manifest.h"

/*
 * Initialise dr release manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_release_manifest_init(UmiDrReleaseManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrReleaseManifest){0};  } }
/*
 * Check that dr release manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_release_manifest_valid(const UmiDrReleaseManifest *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->artifact_count>0U && value->platform_count>0U); }
/*
 * Provide the dr release manifest fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_release_manifest_fingerprint(const UmiDrReleaseManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_release_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
