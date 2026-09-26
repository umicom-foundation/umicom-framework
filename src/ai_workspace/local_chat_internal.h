/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_internal.h
 *
 * PURPOSE:
 *   Keep protocol codecs private while allowing focused wire-boundary regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_LOCAL_CHAT_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_CHAT_INTERNAL_H
#include "umicom/ai_workspace/providers.h"
#define AW_HTTP_BODY_CAPACITY (128U * 1024U)
UmiStatus AwLocalChatEncode(const UmiAiRequest *request, char **outJson);
void AwLocalChatFree(char *json);
UmiStatus AwLocalChatDecode(const UmiAiRequest *request, const char *json, size_t length, UmiAiResponse *response);
#endif
