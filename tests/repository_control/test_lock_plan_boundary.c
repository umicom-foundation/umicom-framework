/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_plan_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock plan boundary semantics.
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
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/lock_plan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    UmiRepositoryHeadSet *h = calloc(1U, sizeof(*h));
    UmiRepositoryLockPlan *plan = calloc(1U, sizeof(*plan));
    UmiRepositorySubmodule s; UmiRepositoryLockPolicy p;
    assert(i && h && plan); umi_repository_inventory_init(i); umi_repository_head_set_init(h);
    assert(umi_repository_submodule_init(&s, "framework", "framework", "", "", 1) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_OK);
    assert(umi_repository_head_set_upsert(h, "framework", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == UMI_STATUS_OK);
    umi_repository_lock_policy_init(&p); umi_repository_lock_policy_set_dry_run(&p, 1);
    assert(umi_repository_lock_plan_build(i, h, &p, plan) == UMI_STATUS_OK);
    assert(plan->stage_count == 0U && plan->dry_run);
    free(plan); free(h); free(i);
    return 0;
}
