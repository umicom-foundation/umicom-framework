/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/lock_plan.c
 *
 * PURPOSE:
 *   Build an explicit lock plan before any parent index mutation occurs.
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
#include "umicom/repository/lock_plan.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the repository lock action text operation used by this module and its client
 * applications.
 */
const char *umi_repository_lock_action_text(UmiRepositoryLockAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_REPOSITORY_LOCK_NO_CHANGE: return "no-change";
        case UMI_REPOSITORY_LOCK_STAGE_GITLINK: return "stage-gitlink";
        case UMI_REPOSITORY_LOCK_MISSING_HEAD: return "missing-head";
        default: return "unknown";
    }
}

/*
 * Provide the repository lock plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_lock_plan_build(
    const UmiRepositoryInventory *inventory,
    const UmiRepositoryHeadSet *heads,
    const UmiRepositoryLockPolicy *policy,
    UmiRepositoryLockPlan *out_plan)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || heads == NULL || policy == NULL ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_repository_lock_policy_validate(policy) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->dry_run = policy->dry_run;
    out_plan->revision = 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->count; ++index) {
        const UmiRepositorySubmodule *submodule = &inventory->items[index];
        const UmiRepositoryHead *head =
            umi_repository_head_set_find(heads, submodule->path);
        UmiRepositoryLockStep *step;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_plan->count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        step = &out_plan->steps[out_plan->count++];
        (void)snprintf(step->name, sizeof(step->name), "%s", submodule->name);
        (void)snprintf(step->path, sizeof(step->path), "%s", submodule->path);
        step->required = submodule->required;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (head == NULL) {
            step->action = UMI_REPOSITORY_LOCK_MISSING_HEAD;
            out_plan->missing_count += 1U;
            continue;
        }
        (void)snprintf(step->head, sizeof(step->head), "%s", head->sha);
        /* Apply this branch only when its contract condition is satisfied. */
        if (policy->stage_gitlinks && !policy->dry_run) {
            step->action = UMI_REPOSITORY_LOCK_STAGE_GITLINK;
            out_plan->stage_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            step->action = UMI_REPOSITORY_LOCK_NO_CHANGE;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_all_heads && out_plan->missing_count > 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
