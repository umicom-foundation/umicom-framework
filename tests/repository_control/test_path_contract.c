/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_path_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository path contract semantics.
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
#include "umicom/repository/path.h"

int main(void)
{
    char out[128];
    assert(umi_repository_control_path_is_safe_relative("applications/studio"));
    assert(umi_repository_control_path_normalize("applications\\studio", out, sizeof(out)) == UMI_STATUS_OK);
    assert(strcmp(out, "applications/studio") == 0);
    return 0;
}
