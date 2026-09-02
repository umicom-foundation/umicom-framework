/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_policy_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock policy invalid semantics.
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
#include "umicom/repository/lock_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryLockPolicy p;
    umi_repository_lock_policy_init(&p);
    p.dry_run = 1;
    assert(umi_repository_lock_policy_validate(&p) == UMI_STATUS_INVALID_STATE);
    return 0;
}
