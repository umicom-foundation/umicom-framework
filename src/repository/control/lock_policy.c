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

/*
 * Initialise repository lock policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_lock_policy_init(UmiRepositoryLockPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->stage_gitlinks = 1;
    policy->require_all_heads = 1;
    policy->verify_after_stage = 1;
}

/*
 * Perform repository lock policy set dry through the module contract so client
 * applications do not duplicate its policy.
 */
void umi_repository_lock_policy_set_dry_run(
    UmiRepositoryLockPolicy *policy, int dry_run)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    policy->dry_run = dry_run != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->dry_run) policy->stage_gitlinks = 0;
}

/*
 * Check that repository lock policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_lock_policy_validate(
    const UmiRepositoryLockPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->dry_run && policy->stage_gitlinks) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
