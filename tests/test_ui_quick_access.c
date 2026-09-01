/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_quick_access.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static UmiStatus handler(void *user_data,
                         const char *argument,
                         char *out_message,
                         size_t capacity)
{
    (void)user_data;
    (void)argument;
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", "opened");
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiActionModel *actions = NULL;
    UmiCommandDescriptor descriptor = {0};
    UmiUiActionSnapshot action = {0};
    UmiUiQuickAccessResults results;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_action_model_create(&actions) == UMI_STATUS_OK);
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "studio.file.quick-open";
    descriptor.title = "Quick Open File";
    descriptor.category = "File";
    descriptor.description = "Fuzzy-open a workspace file";
    descriptor.handler = handler;
    assert(umi_command_registry_register(commands, &descriptor) == UMI_STATUS_OK);

    assert(umi_ui_quick_access_search_commands(commands, "quick",
                                                &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(strcmp(results.items[0].command_id, "studio.file.quick-open") == 0);

    (void)snprintf(action.action_id, sizeof(action.action_id), "%s",
                   "studio.action.file.quick-open");
    (void)snprintf(action.command_id, sizeof(action.command_id), "%s",
                   "studio.file.quick-open");
    (void)snprintf(action.label, sizeof(action.label), "%s",
                   "Open Recent Workspace");
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s",
                   "Search recently opened workspace files");
    (void)snprintf(action.icon_name, sizeof(action.icon_name), "%s",
                   "document-open-recent-symbolic");
    (void)snprintf(action.accelerator, sizeof(action.accelerator), "%s",
                   "Ctrl+E");
    action.enabled = 1;
    action.visible = 1;
    assert(umi_ui_action_model_upsert(actions, &action) == UMI_STATUS_OK);

    assert(umi_ui_quick_access_search_actions(commands, actions, "recent",
                                               &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(strcmp(results.items[0].action_id,
                  "studio.action.file.quick-open") == 0);
    assert(strcmp(results.items[0].title, "Open Recent Workspace") == 0);
    assert(strcmp(results.items[0].accelerator, "Ctrl+E") == 0);
    assert(strcmp(results.items[0].icon_name,
                  "document-open-recent-symbolic") == 0);

    umi_ui_action_model_destroy(actions);
    umi_command_registry_destroy(commands);
    return 0;
}
