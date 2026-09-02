/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_submodule_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository submodule contract semantics.
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
    assert(umi_repository_submodule_init(&s, "framework", "framework",
        "https://example.invalid/framework.git", "main", 1) == UMI_STATUS_OK);
    assert(umi_repository_submodule_validate(&s) == UMI_STATUS_OK);
    assert(s.required);
    return 0;
}
