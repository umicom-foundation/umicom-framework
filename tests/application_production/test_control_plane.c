/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_control_plane.c
 *
 * PURPOSE:
 *   Implement the test control plane behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | control_plane | Sammy Hegab | Umicom Foundation | MIT */
#include <stdlib.h>
#include "test_fixture.h"
#include "umicom/test_runtime/check.h"

/* Verify that the portfolio control plane owns one matching registry entry
 * for each runtime and can resolve a known application by its canonical ID. */
int main(void)
{
    UmiApplicationProductionControlPlane *plane =
        (UmiApplicationProductionControlPlane *)calloc(1U, sizeof(*plane));

    UMI_TEST_REQUIRE(plane != NULL);
    UMI_TEST_REQUIRE(umi_application_production_control_plane_init(
        umi_test_application_production_all_available, NULL, plane) ==
        UMI_STATUS_OK);
    UMI_TEST_REQUIRE(plane->registry.count == plane->portfolio.count);
    UMI_TEST_REQUIRE(umi_application_production_control_plane_runtime(
        plane, "org.umicom.studio") != NULL);
    free(plane);
    return 0;
}
