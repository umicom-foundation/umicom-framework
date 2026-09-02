/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_preferences_persistence.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience preferences persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/preferences_persistence.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiDeveloperPreferences source;
    UmiAiDeveloperPreferences restored;
    int did_restore = 0;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    umi_ai_developer_preferences_init(&source);
    source.diff_layout = UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED;
    source.visible_rows = 40U;
    source.show_tool_arguments = 1;

    assert(umi_ai_developer_preferences_save(
        store, "preferences", &source) == UMI_STATUS_OK);
    assert(umi_ai_developer_preferences_restore(
        store, "preferences", &restored, &did_restore) == UMI_STATUS_OK);
    assert(did_restore);
    assert(restored.diff_layout == UMI_AI_DEVELOPER_DIFF_LAYOUT_UNIFIED);
    assert(restored.visible_rows == 40U);
    assert(restored.show_tool_arguments == 1);

    umi_session_store_destroy(store);
    return 0;
}

