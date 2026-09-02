/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_chat_session.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for chat session.
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
#include "umicom/ai_coding_tools/chat_session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingToolChatSession session;
    UmiAiRequest request;

    assert(umi_ai_coding_tool_chat_session_init(
        &session, "chat", "provider", "model") == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_add(
        &session, UMI_AI_ROLE_USER, "user", "hello") == UMI_STATUS_OK);
    assert(session.turn_count == 1U);
    assert(umi_ai_coding_tool_chat_build_request(
        &session, 2048U, 0.1, &request) == UMI_STATUS_OK);
    assert(request.message_count == 1U);
    assert(strcmp(request.messages[0].text, "hello") == 0);
    return 0;
}

