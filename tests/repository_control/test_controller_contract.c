/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_controller_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository controller contract semantics.
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
#include "umicom/repository/controller.h"

int main(void)
{
    UmiRepositoryController c;
    umi_repository_controller_init(&c);
    assert(umi_repository_controller_begin(&c,0)==UMI_STATUS_OK);
    assert(umi_repository_controller_mark_planned(&c)==UMI_STATUS_OK);
    assert(umi_repository_controller_can_stage(&c));
    assert(umi_repository_controller_mark_staging(&c)==UMI_STATUS_OK);
    assert(umi_repository_controller_mark_verified(&c)==UMI_STATUS_OK);
    return 0;
}
