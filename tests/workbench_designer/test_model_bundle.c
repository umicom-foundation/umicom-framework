/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_model_bundle.c
 *
 * PURPOSE:
 *   Verify one immutable frontend bundle captures service, property, validation,
 *   command and preview projections.
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
    UmiWorkbenchDesignerService *service;
    UmiWorkbenchDesignerSession *session;
    UmiWorkbenchDesignerController controller;
    UmiWorkbenchDesignerKeymap keymap;
    UmiWorkbenchDesignerModelBundle bundle;

    TEST_REQUIRE_STATUS(test_make_controller(
        &service, &controller, &session));
    TEST_REQUIRE_STATUS(umi_workbench_designer_session_select(
        session, "editor", false, false));
    umi_workbench_designer_keymap_init(&keymap);
    TEST_REQUIRE_STATUS(umi_workbench_designer_keymap_seed_defaults(&keymap));
    umi_workbench_designer_model_bundle_init(&bundle);
    TEST_REQUIRE_STATUS(umi_workbench_designer_model_bundle_capture(
        &bundle, &controller, &keymap, 5000U));
    TEST_REQUIRE(bundle.service_snapshot.session_count == 1U);
    TEST_REQUIRE(bundle.breadcrumbs.count >= 2U);
    TEST_REQUIRE(bundle.property_groups.count >= 1U);
    TEST_REQUIRE(bundle.command_palette.count >= 10U);
    TEST_REQUIRE(bundle.browser_preview.available);
    TEST_REQUIRE(bundle.captured_at_ms == 5000U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_stop(&controller));
    umi_workbench_designer_service_destroy(service);
    return 0;
}
