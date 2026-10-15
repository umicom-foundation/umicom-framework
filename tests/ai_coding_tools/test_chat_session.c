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
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/ai_coding_tools/chat_session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted chat session value. */
static int UmiAiCodingToolChatSessionConstructionCases(void)
{
    UmiAiCodingToolChatSession *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, "chat", "provider", "model") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->session_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, oversized, "provider", "model") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, value->session_id, "provider", "model") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session_id, "chat") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->provider_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, "chat", oversized, "model") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, "chat", value->provider_id, "model") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->provider_id, "provider") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->model_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, "chat", "provider", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_tool_chat_session_init(value, "chat", "provider", value->model_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->model_id, "model") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiAiCodingToolChatSessionConstructionCases() != 0) return 1;

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

