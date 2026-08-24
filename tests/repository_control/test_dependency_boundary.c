/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_dependency_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository dependency boundary semantics.
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
#include "umicom/repository/dependency.h"

int main(void)
{
    UmiRepositoryDependency d;
    assert(umi_repository_dependency_init(&d, "studio", "applications\\studio", 0) == UMI_STATUS_OK);
    assert(strcmp(d.path, "applications/studio") == 0);
    return 0;
}
