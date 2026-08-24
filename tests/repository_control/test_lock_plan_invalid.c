/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_plan_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock plan invalid semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/lock_plan.h"

int main(void)
{
    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    UmiRepositoryHeadSet *h = calloc(1U, sizeof(*h));
    UmiRepositoryLockPlan *plan = calloc(1U, sizeof(*plan));
    UmiRepositorySubmodule s; UmiRepositoryLockPolicy p;
    assert(i && h && plan); umi_repository_inventory_init(i); umi_repository_head_set_init(h);
    assert(umi_repository_submodule_init(&s, "framework", "framework", "", "", 1) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_OK);
    umi_repository_lock_policy_init(&p);
    assert(umi_repository_lock_plan_build(i, h, &p, plan) == UMI_STATUS_NOT_FOUND);
    free(plan); free(h); free(i);
    return 0;
}
