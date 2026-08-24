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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/lock_plan.h"

#include <stdio.h>
#include <string.h>

const char *umi_repository_lock_action_text(UmiRepositoryLockAction action)
{
    switch (action) {
        case UMI_REPOSITORY_LOCK_NO_CHANGE: return "no-change";
        case UMI_REPOSITORY_LOCK_STAGE_GITLINK: return "stage-gitlink";
        case UMI_REPOSITORY_LOCK_MISSING_HEAD: return "missing-head";
        default: return "unknown";
    }
}

UmiStatus umi_repository_lock_plan_build(
    const UmiRepositoryInventory *inventory,
    const UmiRepositoryHeadSet *heads,
    const UmiRepositoryLockPolicy *policy,
    UmiRepositoryLockPlan *out_plan)
{
    size_t index;
    if (inventory == NULL || heads == NULL || policy == NULL ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_repository_lock_policy_validate(policy) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->dry_run = policy->dry_run;
    out_plan->revision = 1U;
    for (index = 0U; index < inventory->count; ++index) {
        const UmiRepositorySubmodule *submodule = &inventory->items[index];
        const UmiRepositoryHead *head =
            umi_repository_head_set_find(heads, submodule->path);
        UmiRepositoryLockStep *step;
        if (out_plan->count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        step = &out_plan->steps[out_plan->count++];
        (void)snprintf(step->name, sizeof(step->name), "%s", submodule->name);
        (void)snprintf(step->path, sizeof(step->path), "%s", submodule->path);
        step->required = submodule->required;
        if (head == NULL) {
            step->action = UMI_REPOSITORY_LOCK_MISSING_HEAD;
            out_plan->missing_count += 1U;
            continue;
        }
        (void)snprintf(step->head, sizeof(step->head), "%s", head->sha);
        if (policy->stage_gitlinks && !policy->dry_run) {
            step->action = UMI_REPOSITORY_LOCK_STAGE_GITLINK;
            out_plan->stage_count += 1U;
        } else {
            step->action = UMI_REPOSITORY_LOCK_NO_CHANGE;
        }
    }
    if (policy->require_all_heads && out_plan->missing_count > 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
