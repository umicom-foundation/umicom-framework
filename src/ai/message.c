/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/message.c
 *
 * PURPOSE:
 *   Create bounded, role-labelled AI messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This copy step makes ownership explicit at the Framework boundary and prevents providers from keeping pointers into caller memory.
 */

#include "umicom/ai/message.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_message_set(UmiAiMessage *message,
                             UmiAiRole role,
                             const char *name,
                             const char *text)
{
    int written;
    if (message == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(message, 0, sizeof(*message));
    message->role = role;
    if (name != NULL) {
        written = snprintf(message->name, sizeof(message->name), "%s", name);
        if (written < 0 || (size_t)written >= sizeof(message->name)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    written = snprintf(message->text, sizeof(message->text), "%s", text);
    return written >= 0 && (size_t)written < sizeof(message->text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
