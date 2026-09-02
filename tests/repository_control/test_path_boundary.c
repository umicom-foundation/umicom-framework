/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_path_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository path boundary semantics.
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
    char out[4];
    assert(umi_repository_control_path_normalize("framework", out, sizeof(out)) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(!umi_repository_control_path_is_safe_relative("a//b"));
    return 0;
}
