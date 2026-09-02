/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_presentation_state.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience presentation state.
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
#include "umicom/ai_developer_experience/presentation_state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperPresentationState state;

    umi_ai_developer_presentation_state_init(&state);
    assert(state.active_pane == UMI_AI_DEVELOPER_PANE_OVERVIEW);

    assert(umi_ai_developer_presentation_set_pane(
        &state, UMI_AI_DEVELOPER_PANE_CHAT) == UMI_STATUS_OK);
    assert(umi_ai_developer_presentation_set_chat(
        &state, "chat.1") == UMI_STATUS_OK);
    assert(strcmp(state.active_chat_id, "chat.1") == 0);

    assert(umi_ai_developer_presentation_set_diff(
        &state, "src/a.c", 7U) == UMI_STATUS_OK);
    assert(state.active_diff_line == 7U);
    return 0;
}

