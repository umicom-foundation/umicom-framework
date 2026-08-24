/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_service_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository service boundary semantics.
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
#include "umicom/repository/service.h"

int main(void)
{
    UmiRepositoryControlService *s = calloc(1U, sizeof(*s));
    UmiRepositoryLockPolicy p; UmiRepositoryControlSnapshot snap;
    assert(s != NULL); umi_repository_control_service_init(s);
    assert(umi_repository_control_service_load_gitmodules(s,
     "[submodule \"framework\"]\n path = framework\n")==UMI_STATUS_OK);
    assert(umi_repository_control_service_register_head(s,"framework","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")==UMI_STATUS_OK);
    umi_repository_lock_policy_init(&p); umi_repository_lock_policy_set_dry_run(&p,1);
    assert(umi_repository_control_service_plan(s,&p)==UMI_STATUS_OK);
    assert(umi_repository_control_service_snapshot(s,&snap)==UMI_STATUS_OK);
    assert(snap.dry_run && snap.health.stage_count==0U);
    free(s);
    return 0;
}
