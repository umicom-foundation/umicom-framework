/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/message.h
 *
 * PURPOSE:
 *   Define one role-labelled AI conversation message with bounded owned text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Messages own bounded copies of text so callers do not need to keep temporary prompt buffers alive.
 */

#ifndef INCLUDE_UMICOM_AI_MESSAGE_H
#define INCLUDE_UMICOM_AI_MESSAGE_H

#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMessage {
    UmiAiRole role;
    char name[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_TEXT_CAPACITY];
} UmiAiMessage;

UmiStatus umi_ai_message_set(UmiAiMessage *message,
                             UmiAiRole role,
                             const char *name,
                             const char *text);

#ifdef __cplusplus
}
#endif

#endif
