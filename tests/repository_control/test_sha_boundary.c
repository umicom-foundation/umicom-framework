/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_sha_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository sha boundary semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/sha.h"

int main(void)
{
    char out[41];
    assert(umi_repository_sha_normalize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", out, sizeof(out)) == UMI_STATUS_OK);
    assert(strlen(out) == 40U);
    assert(umi_repository_sha_normalize("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", out, sizeof(out)) == UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
