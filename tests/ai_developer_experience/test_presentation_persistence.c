/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_presentation_persistence.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience presentation persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/presentation_persistence.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiDeveloperPresentationState source;
    UmiAiDeveloperPresentationState restored;
    int did_restore = 0;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    umi_ai_developer_presentation_state_init(&source);
    assert(umi_ai_developer_presentation_set_pane(
        &source, UMI_AI_DEVELOPER_PANE_DIFF) == UMI_STATUS_OK);
    assert(umi_ai_developer_presentation_set_chat(
        &source, "chat.1") == UMI_STATUS_OK);
    assert(umi_ai_developer_presentation_set_diff(
        &source, "src/main.c", 12U) == UMI_STATUS_OK);

    assert(umi_ai_developer_presentation_save(
        store, "presentation", &source) == UMI_STATUS_OK);
    assert(umi_ai_developer_presentation_restore(
        store, "presentation", &restored, &did_restore) == UMI_STATUS_OK);
    assert(did_restore);
    assert(restored.active_pane == UMI_AI_DEVELOPER_PANE_DIFF);
    assert(strcmp(restored.active_chat_id, "chat.1") == 0);
    assert(restored.active_diff_line == 12U);

    umi_session_store_destroy(store);
    return 0;
}

