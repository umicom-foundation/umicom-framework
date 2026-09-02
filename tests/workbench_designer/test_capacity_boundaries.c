/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_capacity_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded designer collections reject overflow without corrupting
 *   existing state.
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
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchDesignerPaletteItem item;
    size_t index;
    char identifier[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];

    umi_workbench_designer_selection_init(&selection);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS; ++index) {
        (void)snprintf(identifier, sizeof(identifier), "node.%zu", index);
        TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
            &selection, identifier, index == 0U));
    }
    TEST_REQUIRE(selection.count == UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS);
    TEST_REQUIRE(umi_workbench_designer_selection_add(
        &selection, "node.overflow", false) == UMI_STATUS_CAPACITY_EXCEEDED);
    TEST_REQUIRE(selection.count == UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS);

    umi_workbench_designer_palette_init(&palette);
    (void)memset(&item, 0, sizeof(item));
    item.node_kind = UMI_WORKBENCH_LAYOUT_NODE_PANEL;
    item.default_dock = UMI_WORKBENCH_LAYOUT_DOCK_LEFT;
    item.enabled = true;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_WORKBENCH_DESIGNER_MAX_PALETTE_ITEMS; ++index) {
        (void)snprintf(item.item_id, sizeof(item.item_id), "item.%zu", index);
        (void)snprintf(
            item.component_id, sizeof(item.component_id), "component.%zu", index);
        TEST_REQUIRE_STATUS(test_copy_text(
            item.owner_application_id, sizeof(item.owner_application_id),
            "org.umicom.test"));
        TEST_REQUIRE_STATUS(test_copy_text(
            item.label, sizeof(item.label), "Test Panel"));
        TEST_REQUIRE_STATUS(umi_workbench_designer_palette_add(&palette, &item));
    }
    TEST_REQUIRE(umi_workbench_designer_palette_add(
        &palette, &item) == UMI_STATUS_CAPACITY_EXCEEDED ||
        umi_workbench_designer_palette_add(
            &palette, &item) == UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
