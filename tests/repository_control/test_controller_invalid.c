/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_controller_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository controller invalid semantics.
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
#include "umicom/repository/controller.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryController c;
    umi_repository_controller_init(&c);
    assert(umi_repository_controller_mark_staging(&c)==UMI_STATUS_INVALID_STATE);
    assert(umi_repository_controller_fail(&c,UMI_STATUS_OK)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
