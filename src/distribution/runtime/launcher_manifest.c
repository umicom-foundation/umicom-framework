/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/launcher_manifest.c
 *
 * PURPOSE:
 *   launcher executable, arguments and working-directory contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launcher_manifest.h"

/*
 * Initialise dr launcher manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_launcher_manifest_init(UmiDrLauncherManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLauncherManifest){0};  } }
/*
 * Check that dr launcher manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_launcher_manifest_valid(const UmiDrLauncherManifest *value) { return value != NULL && (value->id[0] != '\0' && value->executable[0] != '\0'); }
/*
 * Provide the dr launcher manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_launcher_manifest_fingerprint(const UmiDrLauncherManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_launcher_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
