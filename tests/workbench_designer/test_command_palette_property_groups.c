/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_command_palette_property_groups.c
 *
 * PURPOSE:
 *   Verify command discoverability, keyboard movement and grouped property-
 *   inspector sections.
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
    UmiWorkbenchDesignerKeymap keymap;
    UmiWorkbenchDesignerCommandPalette palette;
    const UmiWorkbenchDesignerCommandPaletteItem *selected;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerPropertyModel properties;
    UmiWorkbenchDesignerPropertyGroups groups;

    umi_workbench_designer_keymap_init(&keymap);
    TEST_REQUIRE_STATUS(umi_workbench_designer_keymap_seed_defaults(&keymap));
    umi_workbench_designer_command_palette_init(&palette);
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_palette_seed(
        &palette, &keymap));
    TEST_REQUIRE(palette.count >= 10U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_palette_filter(
        &palette, "split"));
    TEST_REQUIRE(umi_workbench_designer_command_palette_visible_count(
        &palette) >= 2U);
    selected = umi_workbench_designer_command_palette_selected(&palette);
    TEST_REQUIRE(selected != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_designer_command_palette_move_selection(
        &palette, 1));

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_property_model_init(&properties);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_model_build(
        &properties, &document, "editor"));
    umi_workbench_designer_property_groups_init(&groups);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_groups_build(
        &groups, &properties));
    TEST_REQUIRE(groups.count >= 2U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_groups_set_expanded(
        &groups, groups.groups[0].group_id, false));
    TEST_REQUIRE(!groups.groups[0].expanded);
    return 0;
}
