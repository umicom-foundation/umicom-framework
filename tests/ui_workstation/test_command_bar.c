/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_command_bar.c
 *
 * PURPOSE:
 *   Verify prefix parsing, safe action registration, ranked filtering,
 *   keyboard selection and responsive command-bar presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "umicom/ui/workstation/command_bar.h"

int main(void)
{
    UmiWsCommandBarModel model;
    const UmiWsCommandBarItem *item;
    UmiWsCommandBarQuery q;

    /* Prefixes narrow search while leaving the text clean for matching. */
    if (umi_ws_command_bar_parse("+ Memory", &q) != UMI_STATUS_OK) return 1;
    if (q.scope != UMI_WS_COMMAND_SCOPE_PANEL || q.text[0] != 'M') return 2;
    if (umi_ws_command_bar_scope_prefix(UMI_WS_COMMAND_SCOPE_AI) != '?') return 3;

    umi_ws_command_bar_model_init(&model);
    if (model.revision == 0U || model.count != 0U) return 4;
    if (umi_ws_command_bar_model_add(
            &model,
            "window.memory",
            "Memory",
            "Open the memory inspection window.",
            "studio.window.memory",
            "panel diagnostics",
            UMI_WS_COMMAND_SCOPE_PANEL,
            50U) != UMI_STATUS_OK) {
        return 5;
    }
    if (umi_ws_command_bar_model_add(
            &model,
            "command.build",
            "Build Project",
            "Compile and link the active project.",
            "studio.build",
            "compile project",
            UMI_WS_COMMAND_SCOPE_COMMAND,
            100U) != UMI_STATUS_OK) {
        return 6;
    }
    if (umi_ws_command_bar_model_add(
            &model,
            "command.build",
            "Duplicate",
            "This item must be rejected.",
            "studio.duplicate",
            "duplicate",
            UMI_WS_COMMAND_SCOPE_COMMAND,
            1U) != UMI_STATUS_ALREADY_EXISTS) {
        return 7;
    }
    if (model.count != 2U) return 8;

    /* The higher-priority action is first for an empty all-scope query. */
    item = umi_ws_command_bar_model_selected(&model);
    if (item == NULL || strcmp(item->item_id, "command.build") != 0) return 9;
    if (umi_ws_command_bar_model_set_query(&model, "+ mem") != UMI_STATUS_OK)
        return 10;
    if (model.result_count != 1U ||
        model.query.scope != UMI_WS_COMMAND_SCOPE_PANEL) {
        return 11;
    }
    item = umi_ws_command_bar_model_selected(&model);
    if (item == NULL || strcmp(item->command_id, "studio.window.memory") != 0)
        return 12;

    /* Disabled actions remain discoverable, but carry explicit state that a
     * frontend must honour before dispatching the command. */
    if (umi_ws_command_bar_model_set_enabled(
            &model, "window.memory", false) != UMI_STATUS_OK) {
        return 13;
    }
    item = umi_ws_command_bar_model_selected(&model);
    if (item == NULL || item->enabled) return 14;
    if (umi_ws_command_bar_model_move_selection(&model, 1) != UMI_STATUS_OK)
        return 15;
    if (model.selected_result != 0U) return 16;

    /* Width policy changes presentation only; no registered action is lost. */
    if (umi_ws_command_bar_model_set_available_width(&model, 360) !=
        UMI_STATUS_OK) return 17;
    if (model.presentation != UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED)
        return 18;
    if (umi_ws_command_bar_model_set_available_width(&model, 220) !=
        UMI_STATUS_OK) return 19;
    if (model.presentation != UMI_WS_COMMAND_BAR_PRESENTATION_COMPACT)
        return 20;
    if (umi_ws_command_bar_model_set_available_width(&model, 100) !=
        UMI_STATUS_OK) return 21;
    if (model.presentation != UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON ||
        model.count != 2U) {
        return 22;
    }

    puts("command bar: ok");
    return 0;
}
