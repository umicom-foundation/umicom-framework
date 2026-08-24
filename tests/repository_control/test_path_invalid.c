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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/path.h"

int main(void)
{
    assert(!umi_repository_control_path_is_safe_relative("../framework"));
    assert(!umi_repository_control_path_is_safe_relative("C:\\framework"));
    assert(!umi_repository_control_path_is_safe_relative("/framework"));
    return 0;
}
