/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_ref_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository ref contract semantics.
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
#include "umicom/repository/ref.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char out[64];
    assert(umi_repository_ref_is_valid("main"));
    assert(umi_repository_ref_is_valid("feature/native-repo"));
    assert(umi_repository_ref_copy("release/v1", out, sizeof(out)) == UMI_STATUS_OK);
    return 0;
}
