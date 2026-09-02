/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency invalid semantics.
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
#include <string.h>
#include "umicom/repository/dependency.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryDependency d;
    assert(umi_repository_dependency_init(&d, "", "framework", 1) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_repository_dependency_init(&d, "x", "../x", 1) != UMI_STATUS_OK);
    return 0;
}
