/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_path_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository path invalid semantics.
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
#include "umicom/repository/path.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(!umi_repository_control_path_is_safe_relative("../framework"));
    assert(!umi_repository_control_path_is_safe_relative("C:\\framework"));
    assert(!umi_repository_control_path_is_safe_relative("/framework"));
    return 0;
}
