/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_health_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository health invalid semantics.
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
#include "umicom/repository/health.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_repository_health_from_plan(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
