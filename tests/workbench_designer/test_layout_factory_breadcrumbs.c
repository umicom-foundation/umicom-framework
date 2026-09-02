/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_layout_factory_breadcrumbs.c
 *
 * PURPOSE:
 *   Verify starter-layout composition and root-to-selection breadcrumb
 *   navigation.
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
    UmiWorkbenchDesignerLayoutFactoryRequest request =
        umi_workbench_designer_layout_factory_request_default();
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerBreadcrumbs breadcrumbs;
    const UmiWorkbenchDesignerBreadcrumb *last;

    request.profile = UMI_WORKBENCH_DESIGNER_LAYOUT_CODING;
    TEST_REQUIRE_STATUS(test_copy_text(
        request.layout_id, sizeof(request.layout_id), "layout.coding"));
    TEST_REQUIRE_STATUS(test_copy_text(
        request.name, sizeof(request.name), "Coding"));
    TEST_REQUIRE_STATUS(test_copy_text(
        request.owner_user_id, sizeof(request.owner_user_id), "user.sammy"));
    umi_workbench_designer_palette_init(&palette);
    TEST_REQUIRE_STATUS(umi_workbench_designer_palette_seed_framework(&palette));
    TEST_REQUIRE_STATUS(umi_workbench_designer_layout_factory_create(
        &request, &palette, &document));
    TEST_REQUIRE(document.node_count >= 5U);
    TEST_REQUIRE(umi_workbench_layout_document_root(&document) != NULL);
    umi_workbench_designer_breadcrumbs_init(&breadcrumbs);
    TEST_REQUIRE_STATUS(umi_workbench_designer_breadcrumbs_build(
        &breadcrumbs, &document, "editor"));
    TEST_REQUIRE(breadcrumbs.count >= 2U);
    last = umi_workbench_designer_breadcrumb_at(
        &breadcrumbs, breadcrumbs.count - 1U);
    TEST_REQUIRE(last != NULL && last->active);
    TEST_REQUIRE(strcmp(last->node_id, "editor") == 0);
    return 0;
}
