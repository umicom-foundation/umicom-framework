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

void umi_dr_launcher_manifest_init(UmiDrLauncherManifest *value) { if (value != NULL) { *value = (UmiDrLauncherManifest){0};  } }
bool umi_dr_launcher_manifest_valid(const UmiDrLauncherManifest *value) { return value != NULL && (value->id[0] != '\0' && value->executable[0] != '\0'); }
uint64_t umi_dr_launcher_manifest_fingerprint(const UmiDrLauncherManifest *value) {
    uint64_t h = 0U;
    if (!umi_dr_launcher_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
