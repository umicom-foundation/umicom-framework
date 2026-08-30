/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_chat_registry.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for chat registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_coding_tools/chat_registry.h"

int main(void)
{
    UmiAiCodingToolChatRegistry registry;
    UmiAiCodingToolChatSession *session = NULL;

    umi_ai_coding_tool_chat_registry_init(&registry);
    assert(umi_ai_coding_tool_chat_registry_open(
        &registry, "chat", "provider", "model", &session) == UMI_STATUS_OK);
    assert(session != NULL);
    assert(registry.count == 1U);
    assert(umi_ai_coding_tool_chat_registry_find(
        &registry, "chat") == session);
    assert(umi_ai_coding_tool_chat_registry_close(
        &registry, "chat") == UMI_STATUS_OK);
    assert(registry.count == 0U);
    return 0;
}

