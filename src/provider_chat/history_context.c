/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/history_context.c
 * PURPOSE: Compose only caller-selected UTF-8 excerpts as ordinary reviewed context.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "history_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
static bool Boundary(const char *text, size_t offset)
{
    return ((unsigned char)text[offset] & 0xc0U) != 0x80U;
}
UmiStatus UmiProviderChatHistoryCompose(const UmiProviderChatHistory *history, uint64_t expected_revision,
                                        const UmiProviderChatHistoryExcerpt *excerpts, size_t count,
                                        char *out_text, size_t capacity)
{
    if (history == NULL || excerpts == NULL || count == 0U || count > UMI_PROVIDER_CHAT_HISTORY_EXCERPTS ||
        out_text == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->revision != expected_revision)
        return UMI_STATUS_BUSY;
    size_t bound =
        capacity < UMI_PROVIDER_CHAT_CONTEXT_CAPACITY ? capacity : UMI_PROVIDER_CHAT_CONTEXT_CAPACITY;
    char *draft = calloc(bound, 1U);
    if (draft == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i)
    {
        const UmiProviderChatHistoryExcerpt *part = &excerpts[i];
        const UmiProviderChatHistoryEntry *entry = UmiProviderChatHistoryFind(history, part->entry_id);
        if (entry == NULL)
        {
            status = UMI_STATUS_NOT_FOUND;
            break;
        }
        if (part->role != UMI_AI_ROLE_USER && part->role != UMI_AI_ROLE_ASSISTANT)
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        const char *text = part->role == UMI_AI_ROLE_USER ? entry->request : entry->reply;
        size_t length = strlen(text), offset = part->byte_offset, bytes = part->byte_count;
        if (bytes == 0U || offset > length || bytes > length - offset || !Boundary(text, offset) ||
            !Boundary(text, offset + bytes))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        const char *label = part->role == UMI_AI_ROLE_USER ? "Earlier user text (quoted context):\n"
                            : entry->refused && entry->truncated
                                ? "Earlier assistant refusal, incomplete (quoted context):\n"
                            : entry->refused   ? "Earlier assistant refusal (quoted context):\n"
                            : entry->truncated ? "Earlier assistant text, incomplete (quoted context):\n"
                                               : "Earlier assistant text (quoted context):\n";
        size_t label_bytes = strlen(label), separator = i == 0U ? 0U : 2U;
        /* Each subtraction leaves room for the final terminator. Build into
         * temporary storage so a bad later excerpt cannot publish a prefix. */
        size_t remaining = bound - 1U - used;
        if (separator > remaining || label_bytes > remaining - separator ||
            bytes > remaining - separator - label_bytes)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (separator != 0U)
        {
            memcpy(draft + used, "\n\n", 2U);
            used += 2U;
        }
        memcpy(draft + used, label, label_bytes);
        used += label_bytes;
        memcpy(draft + used, text + offset, bytes);
        used += bytes;
        draft[used] = '\0';
    }
    if (status == UMI_STATUS_OK)
        memcpy(out_text, draft, used + 1U);
    umi_secret_clear(draft, bound);
    free(draft);
    return status;
}
