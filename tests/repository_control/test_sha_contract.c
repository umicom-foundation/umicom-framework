/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_sha_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository sha contract semantics.
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
#include "umicom/repository/sha.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char out[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    assert(umi_repository_sha_is_valid("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    assert(umi_repository_sha_normalize("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", out, sizeof(out)) == UMI_STATUS_OK);
    assert(out[0] == 'b');
    return 0;
}
