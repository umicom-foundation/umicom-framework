/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_submodule_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository submodule invalid semantics.
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
#include "umicom/repository/submodule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositorySubmodule s;
    assert(umi_repository_submodule_init(&s, "", "framework", "", "main", 1) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_repository_submodule_init(&s, "x", "../x", "", "main", 1) != UMI_STATUS_OK);
    return 0;
}
