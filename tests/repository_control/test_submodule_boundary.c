/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_submodule_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository submodule boundary semantics.
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
#include "umicom/repository/submodule.h"

int main(void)
{
    UmiRepositorySubmodule s;
    assert(umi_repository_submodule_init(&s, "studio", "applications/studio", "", "", 0) == UMI_STATUS_OK);
    assert(s.branch[0] == '\0');
    assert(!s.required);
    return 0;
}
