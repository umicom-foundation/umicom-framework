/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_service_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository service contract semantics.
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
#include "umicom/repository/service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryControlService *s = calloc(1U, sizeof(*s));
    UmiRepositoryLockPolicy p; UmiRepositoryControlSnapshot snap;
    assert(s != NULL); umi_repository_control_service_init(s);
    assert(umi_repository_control_service_load_gitmodules(s,
     "[submodule \"framework\"]\n path = framework\n")==UMI_STATUS_OK);
    assert(umi_repository_control_service_register_head(s,"framework","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")==UMI_STATUS_OK);
    umi_repository_lock_policy_init(&p);
    assert(umi_repository_control_service_plan(s,&p)==UMI_STATUS_OK);
    assert(umi_repository_control_service_snapshot(s,&snap)==UMI_STATUS_OK);
    assert(snap.health.healthy);
    free(s);
    return 0;
}
