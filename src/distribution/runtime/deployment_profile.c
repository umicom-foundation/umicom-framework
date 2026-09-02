/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/deployment_profile.c
 *
 * PURPOSE:
 *   deployment target, scope, rollout and update-channel profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/deployment_profile.h"

/*
 * Initialise dr deployment profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_deployment_profile_init(UmiDrDeploymentProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDeploymentProfile){0};  } }
/*
 * Check that dr deployment profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_deployment_profile_valid(const UmiDrDeploymentProfile *value) { return value != NULL && (value->id[0] != '\0' && value->target[0] != '\0' && value->scope != 0 && value->channel != 0 && value->rollout_percent<=100U); }
/*
 * Provide the dr deployment profile fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_deployment_profile_fingerprint(const UmiDrDeploymentProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_deployment_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
