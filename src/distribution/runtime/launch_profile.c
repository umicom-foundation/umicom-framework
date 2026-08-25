/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/launch_profile.c
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launch_profile.h"

void umi_dr_launch_profile_init(UmiDrLaunchProfile *value) { if (value != NULL) { *value = (UmiDrLaunchProfile){0};  } }
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value) { return value != NULL && (value->id[0] != '\0' && value->launcher_id[0] != '\0'); }
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value) {
    uint64_t h = 0U;
    if (!umi_dr_launch_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
