/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/preferences.c
 *
 * PURPOSE:
 *   Implement conservative defaults and validation for AI developer UI behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/preferences.h"

#include <string.h>

void umi_ai_developer_preferences_init(
    UmiAiDeveloperPreferences *preferences)
{
    if (preferences == NULL) return;

    (void)memset(preferences, 0, sizeof(*preferences));
    preferences->diff_layout = UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE;
    preferences->diff_context_lines = 3U;
    preferences->visible_rows = 24U;
    preferences->auto_follow_active_task = 1;
    preferences->auto_open_review = 1;
    preferences->show_tool_arguments = 0;
    preferences->show_validation_output = 1;
    preferences->show_context_token_estimates = 1;
    preferences->revision = 1U;
}

UmiStatus umi_ai_developer_preferences_validate(
    const UmiAiDeveloperPreferences *preferences)
{
    if (preferences == NULL ||
        preferences->diff_layout < UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED ||
        preferences->diff_layout > UMI_AI_DEVELOPER_DIFF_LAYOUT_SIDE_BY_SIDE ||
        preferences->diff_context_lines > 20U ||
        preferences->visible_rows == 0U ||
        preferences->visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
