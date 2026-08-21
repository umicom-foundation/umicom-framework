/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_keymap_toolbar_status.c
 *
 * PURPOSE:
 *   Verify default keyboard bindings, toolbar state and status projection remain
 *   consistent.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerKeymap keymap;
    UmiWorkbenchDesignerToolbar toolbar;
    UmiWorkbenchDesignerStatusModel status;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerCollaborationModel collaboration;
    const UmiWorkbenchDesignerKeybinding *binding;
    const UmiWorkbenchDesignerToolbarItem *item;

    umi_workbench_designer_keymap_init(&keymap);
    TEST_REQUIRE_STATUS(umi_workbench_designer_keymap_seed_defaults(&keymap));
    TEST_REQUIRE(keymap.count > 6U);
    binding = umi_workbench_designer_keymap_resolve(
        &keymap, "Ctrl+S", "designer");
    TEST_REQUIRE(binding != NULL);
    TEST_REQUIRE(strcmp(binding->command_id, "designer.save") == 0);

    umi_workbench_designer_toolbar_init(&toolbar);
    TEST_REQUIRE_STATUS(umi_workbench_designer_toolbar_seed(&toolbar));
    TEST_REQUIRE_STATUS(umi_workbench_designer_toolbar_set_state(
        &toolbar, "save", true, true, false));
    item = umi_workbench_designer_toolbar_find(&toolbar, "save");
    TEST_REQUIRE(item != NULL && item->enabled);

    umi_workbench_designer_status_init(&status);
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_collaboration_init(&collaboration);
    TEST_REQUIRE_STATUS(umi_workbench_designer_status_update(
        &status, "Ready", "Fixture Layout",
        UMI_WORKBENCH_DESIGNER_MODE_DESIGN,
        UMI_WORKBENCH_DESIGNER_TOOL_SELECT,
        UMI_WORKBENCH_DESIGNER_SAVE_CLEAN,
        (UmiWorkbenchDesignerPoint){12.0, 24.0}, 1.25,
        &selection, &collaboration, 0U, false, 7U));
    TEST_REQUIRE(status.zoom == 1.25);
    TEST_REQUIRE(strcmp(status.message, "Ready") == 0);
    return 0;
}
