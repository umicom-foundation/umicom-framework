/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_ui/assistant_windows.c
 *
 * PURPOSE:
 *   Populate an application window catalogue with shared AI workspace tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Registering a descriptor makes a window discoverable. It does not start a
 * model, expose a secret or grant an agent permission to change files.
 */
#include "umicom/ai_ui/assistant_windows.h"

#include <stddef.h>

static const UmiUiWindowDescriptor ASSISTANT_WINDOWS[] = {
    {
        .tool_id = UMI_AI_UI_WINDOW_CHAT,
        .title = "Assistant Chat",
        .description = "Hold a contextual conversation with a selected model",
        .icon_name = "mail-message-new-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = true,
        .default_width = 0.30,
        .default_height = 0.70
    },
    {
        .tool_id = UMI_AI_UI_WINDOW_AGENT_TASKS,
        .title = "Agent Tasks",
        .description = "Plan and inspect bounded tasks performed by assistants",
        .icon_name = "view-list-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = false,
        .default_width = 0.34,
        .default_height = 0.55
    },
    {
        .tool_id = UMI_AI_UI_WINDOW_APPROVALS,
        .title = "Assistant Approvals",
        .description = "Review actions that require explicit user permission",
        .icon_name = "security-high-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = false,
        .default_width = 0.36,
        .default_height = 0.45
    },
    {
        .tool_id = UMI_AI_UI_WINDOW_TOOL_ACTIVITY,
        .title = "Tool Activity",
        .description = "Inspect assistant commands, results and validation evidence",
        .icon_name = "utilities-system-monitor-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = false,
        .default_width = 0.55,
        .default_height = 0.32
    },
    {
        .tool_id = UMI_AI_UI_WINDOW_MODEL_COMPARISON,
        .title = "Model Comparison",
        .description = "Compare several approved model answers side by side",
        .icon_name = "view-grid-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = true,
        .default_width = 0.60,
        .default_height = 0.62
    },
    {
        .tool_id = UMI_AI_UI_WINDOW_RESEARCH_CONTEXT,
        .title = "Research and Context",
        .description = "Search governed local knowledge and inspect cited sources",
        .icon_name = "system-search-symbolic",
        .category = UMI_UI_WINDOW_CATEGORY_AI,
        .supports_multiple = true,
        .default_width = 0.40,
        .default_height = 0.60
    }
};

/*
 * Add ai ui assistant windows only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_ui_assistant_windows_register(
    UmiUiWindowCatalogue *catalogue)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Registration stops on the first error so callers receive exact evidence. */
    for (index = 0U;
         index < sizeof(ASSISTANT_WINDOWS) / sizeof(ASSISTANT_WINDOWS[0]);
         ++index) {
        UmiStatus status = umi_ui_window_catalogue_register(
            catalogue, &ASSISTANT_WINDOWS[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}
