/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/conversation.c
 *
 * PURPOSE:
 *   Maintain bounded provider-neutral conversation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The bounded conversation makes capacity pressure visible so Studio can summarise or archive history instead of silently growing memory.
 */

#include "umicom/ai/conversation.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

void umi_ai_conversation_init(UmiAiConversation *conversation, const char *id)
{
    if (conversation != NULL) {
        (void)memset(conversation, 0, sizeof(*conversation));
        if (id != NULL) {
            (void)snprintf(conversation->conversation_id,
                           sizeof(conversation->conversation_id),
                           "%s", id);
        }
    }
}

UmiStatus umi_ai_conversation_add(UmiAiConversation *conversation,
                                  const UmiAiMessage *message)
{
    if (conversation == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (conversation->message_count >= UMI_AI_MAX_MESSAGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    conversation->messages[conversation->message_count++] = *message;
    return UMI_STATUS_OK;
}
