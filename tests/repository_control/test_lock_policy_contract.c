/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_policy_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock policy contract semantics.
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
    assert(p.stage_gitlinks);
    assert(p.require_all_heads);
    assert(umi_repository_lock_policy_validate(&p) == UMI_STATUS_OK);
    return 0;
}
