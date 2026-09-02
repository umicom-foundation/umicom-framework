/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_manifest.c
 *
 * PURPOSE:
 *   published update metadata, version, channel and package fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_manifest.h"

/*
 * Initialise dr update manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_update_manifest_init(UmiDrUpdateManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrUpdateManifest){0};  } }
/*
 * Check that dr update manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_update_manifest_valid(const UmiDrUpdateManifest *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->package_digest[0] != '\0'); }
/*
 * Provide the dr update manifest fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_update_manifest_fingerprint(const UmiDrUpdateManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_update_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
