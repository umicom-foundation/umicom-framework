/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/conversation.h
 *
 * PURPOSE:
 *   Store a bounded conversation independent of any specific provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Conversation state belongs to the application layer, not to a vendor SDK, so providers can be swapped without losing the message history.
 */

#ifndef INCLUDE_UMICOM_AI_CONVERSATION_H
#define INCLUDE_UMICOM_AI_CONVERSATION_H

#include <stddef.h>
#include "umicom/ai/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiConversation {
    char conversation_id[UMI_AI_ID_CAPACITY];
    UmiAiMessage messages[UMI_AI_MAX_MESSAGES];
    size_t message_count;
} UmiAiConversation;

void umi_ai_conversation_init(UmiAiConversation *conversation, const char *id);
UmiStatus umi_ai_conversation_add(UmiAiConversation *conversation,
                                  const UmiAiMessage *message);

#ifdef __cplusplus
}
#endif

#endif
