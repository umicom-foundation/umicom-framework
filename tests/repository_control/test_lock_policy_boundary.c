/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_policy_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock policy boundary semantics.
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
#include "umicom/repository/lock_policy.h"

int main(void)
{
    UmiRepositoryLockPolicy p;
    umi_repository_lock_policy_init(&p);
    umi_repository_lock_policy_set_dry_run(&p, 1);
    assert(p.dry_run);
    assert(!p.stage_gitlinks);
    assert(umi_repository_lock_policy_validate(&p) == UMI_STATUS_OK);
    return 0;
}
