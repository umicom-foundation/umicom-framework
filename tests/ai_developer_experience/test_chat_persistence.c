/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_chat_persistence.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience chat persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/chat_persistence.h"

int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiCodingToolChatSession source;
    UmiAiCodingToolChatSession restored;
    int did_restore = 0;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_session_init(
        &source, "chat.1", "provider", "model") == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_add(
        &source, UMI_AI_ROLE_USER, "user", "hello\nworld") == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_add(
        &source, UMI_AI_ROLE_ASSISTANT, "assistant", "answer") ==
        UMI_STATUS_OK);

    assert(umi_ai_developer_chat_session_save(
        store, "chat.test", &source) == UMI_STATUS_OK);
    assert(umi_ai_developer_chat_session_restore(
        store, "chat.test", &restored, &did_restore) == UMI_STATUS_OK);
    assert(did_restore);
    assert(restored.message_count == 2U);
    assert(strcmp(restored.messages[0].text, "hello\nworld") == 0);

    umi_session_store_destroy(store);
    return 0;
}

