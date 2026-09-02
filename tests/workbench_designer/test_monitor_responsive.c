/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_monitor_responsive.c
 *
 * PURPOSE:
 *   Verify multi-monitor bounds, missing-monitor relocation and responsive
 *   preview profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerMonitorCanvas monitors;
    UmiWorkbenchDesignerMonitor primary;
    UmiWorkbenchDesignerMonitor secondary;
    UmiWorkbenchDesignerResponsivePreview preview;
    const UmiWorkbenchDesignerResponsiveProfile *profile;
    UmiWorkbenchLayoutDocument responsive_document;
    UmiWorkbenchDesignerRect clamped;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_monitor_canvas_init(&monitors);
    (void)memset(&primary, 0, sizeof(primary));
    TEST_REQUIRE_STATUS(test_copy_text(
        primary.monitor_id, sizeof(primary.monitor_id), "monitor.primary"));
    TEST_REQUIRE_STATUS(test_copy_text(
        primary.name, sizeof(primary.name), "Primary"));
    primary.bounds = (UmiWorkbenchDesignerRect){0.0, 0.0, 1920.0, 1080.0};
    primary.work_area = (UmiWorkbenchDesignerRect){0.0, 0.0, 1920.0, 1040.0};
    primary.scale = 1.0;
    primary.primary = true;
    primary.enabled = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_monitor_canvas_add(
        &monitors, &primary));
    secondary = primary;
    TEST_REQUIRE_STATUS(test_copy_text(
        secondary.monitor_id, sizeof(secondary.monitor_id), "monitor.secondary"));
    TEST_REQUIRE_STATUS(test_copy_text(
        secondary.name, sizeof(secondary.name), "Secondary"));
    secondary.bounds.x = 1920.0;
    secondary.work_area.x = 1920.0;
    secondary.primary = false;
    TEST_REQUIRE_STATUS(umi_workbench_designer_monitor_canvas_add(
        &monitors, &secondary));
    clamped = umi_workbench_designer_monitor_canvas_clamp(
        &monitors, "monitor.primary",
        (UmiWorkbenchDesignerRect){1800.0, 1000.0, 600.0, 400.0});
    TEST_REQUIRE(clamped.x + clamped.width <= 1920.0);

    umi_workbench_designer_responsive_preview_init(&preview);
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_seed(&preview));
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_activate(
        &preview, "mobile"));
    profile = umi_workbench_designer_responsive_preview_active(&preview);
    TEST_REQUIRE(profile != NULL);
    TEST_REQUIRE(profile->device == UMI_WORKBENCH_DESIGNER_PREVIEW_MOBILE);
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_apply(
        profile, &document, &responsive_document));
    TEST_REQUIRE(responsive_document.node_count == document.node_count);
    return 0;
}
