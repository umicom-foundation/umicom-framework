/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency contract semantics.
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
    assert(umi_repository_dependency_init(&d, "framework", "framework", 1) == UMI_STATUS_OK);
    assert(umi_repository_dependency_validate(&d) == UMI_STATUS_OK);
    assert(d.required);
    return 0;
}
