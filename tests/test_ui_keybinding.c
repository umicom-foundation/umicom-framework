/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_keybinding.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiUiContextStore *context = NULL;
    UmiUiKeybindingRegistry *bindings = NULL;
    UmiUiKeybindingSnapshot item = {0};
    UmiUiKeybindingResolution resolution;

    assert(umi_ui_context_store_create(&context) == UMI_STATUS_OK);
    assert(umi_ui_keybinding_registry_create(&bindings) == UMI_STATUS_OK);
    assert(umi_ui_context_set_boolean(context, "studio.workspace.open", 1) == UMI_STATUS_OK);

    (void)snprintf(item.binding_id, sizeof(item.binding_id), "%s", "studio.keys.explorer");
    (void)snprintf(item.command_id, sizeof(item.command_id), "%s", "studio.ui.activity.activate");
    (void)snprintf(item.chord, sizeof(item.chord), "%s", "Ctrl+Shift+E");
    (void)snprintf(item.when_expression, sizeof(item.when_expression),
                   "%s", "studio.workspace.open");
    item.enabled = 1;
    item.order = 10;

    assert(umi_ui_keybinding_registry_upsert(bindings, &item) == UMI_STATUS_OK);
    assert(umi_ui_keybinding_registry_resolve(bindings, context,
                                              "Ctrl+Shift+E",
                                              &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id, "studio.ui.activity.activate") == 0);

    umi_ui_keybinding_registry_destroy(bindings);
    umi_ui_context_store_destroy(context);
    return 0;
}
