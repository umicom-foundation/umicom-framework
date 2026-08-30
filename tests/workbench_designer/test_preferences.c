/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_preferences.c
 *
 * PURPOSE:
 *   Verify professional designer defaults, invalid settings rejection and
 *   complete preference overlay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerPreferences defaults =
        umi_workbench_designer_preferences_default();
    UmiWorkbenchDesignerPreferences changed = defaults;
    char error[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    TEST_REQUIRE_STATUS(umi_workbench_designer_preferences_validate(
        &defaults, error, sizeof(error)));
    TEST_REQUIRE(defaults.show_grid && defaults.snap_to_grid);
    changed.default_zoom = 2.0;
    changed.grid_size = 16.0;
    TEST_REQUIRE_STATUS(umi_workbench_designer_preferences_overlay(
        &defaults, &changed));
    TEST_REQUIRE(defaults.default_zoom == 2.0);
    TEST_REQUIRE(umi_workbench_designer_preferences_equal(&defaults, &changed));
    changed.default_zoom = 20.0;
    TEST_REQUIRE(umi_workbench_designer_preferences_validate(
        &changed, error, sizeof(error)) == UMI_STATUS_INVALID_ARGUMENT);
    TEST_REQUIRE(error[0] != '\0');
    return 0;
}
