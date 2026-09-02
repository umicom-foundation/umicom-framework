/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_action_menu.c
 *
 * PURPOSE:
 *   Verify action and menu contribution models preserve stable command references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiActionModel *actions = NULL;
    UmiUiMenuModel *menus = NULL;
    UmiUiActionSnapshot action = {0};
    UmiUiMenuSnapshot menu = {0};
    assert(umi_ui_action_model_create(&actions) == UMI_STATUS_OK);
    assert(umi_ui_menu_model_create(&menus) == UMI_STATUS_OK);
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", "studio.file.open");
    (void)snprintf(action.command_id, sizeof(action.command_id), "%s", "studio.file.open");
    (void)snprintf(action.label, sizeof(action.label), "%s", "Open File");
    action.enabled = 1; action.visible = 1;
    assert(umi_ui_action_model_upsert(actions, &action) == UMI_STATUS_OK);
    (void)snprintf(menu.item_id, sizeof(menu.item_id), "%s", "menu.file.open");
    (void)snprintf(menu.menu_id, sizeof(menu.menu_id), "%s", "menu.file");
    (void)snprintf(menu.section_id, sizeof(menu.section_id), "%s", "file.primary");
    (void)snprintf(menu.action_id, sizeof(menu.action_id), "%s", action.action_id);
    assert(umi_ui_menu_model_upsert(menus, &menu) == UMI_STATUS_OK);
    assert(umi_ui_action_model_count(actions) == 1U);
    assert(umi_ui_menu_model_count(menus) == 1U);
    umi_ui_menu_model_destroy(menus);
    umi_ui_action_model_destroy(actions);
    return EXIT_SUCCESS;
}
