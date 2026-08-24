/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_sha_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository sha invalid semantics.
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
    char out[65];
    assert(!umi_repository_sha_is_valid("abc"));
    assert(umi_repository_sha_normalize("abc", out, sizeof(out)) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
