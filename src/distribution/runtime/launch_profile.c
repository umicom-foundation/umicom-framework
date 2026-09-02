/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/launch_profile.c
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launch_profile.h"

/*
 * Initialise dr launch profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_launch_profile_init(UmiDrLaunchProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLaunchProfile){0};  } }
/* Check that dr launch profile satisfies its contract before another service relies on it. */
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value) { return value != NULL && (value->id[0] != '\0' && value->launcher_id[0] != '\0'); }
/*
 * Provide the dr launch profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_launch_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
