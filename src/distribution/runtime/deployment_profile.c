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

void umi_dr_deployment_profile_init(UmiDrDeploymentProfile *value) { if (value != NULL) { *value = (UmiDrDeploymentProfile){0};  } }
bool umi_dr_deployment_profile_valid(const UmiDrDeploymentProfile *value) { return value != NULL && (value->id[0] != '\0' && value->target[0] != '\0' && value->scope != 0 && value->channel != 0 && value->rollout_percent<=100U); }
uint64_t umi_dr_deployment_profile_fingerprint(const UmiDrDeploymentProfile *value) {
    uint64_t h = 0U;
    if (!umi_dr_deployment_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
