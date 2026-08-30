/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/lock_policy.c
 *
 * PURPOSE:
 *   Define safe native submodule-lock policy including dry-run semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/lock_policy.h"

#include <string.h>

void umi_repository_lock_policy_init(UmiRepositoryLockPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->stage_gitlinks = 1;
    policy->require_all_heads = 1;
    policy->verify_after_stage = 1;
}

void umi_repository_lock_policy_set_dry_run(
    UmiRepositoryLockPolicy *policy, int dry_run)
{
    if (policy == NULL) return;
    policy->dry_run = dry_run != 0;
    if (policy->dry_run) policy->stage_gitlinks = 0;
}

UmiStatus umi_repository_lock_policy_validate(
    const UmiRepositoryLockPolicy *policy)
{
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (policy->dry_run && policy->stage_gitlinks) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
