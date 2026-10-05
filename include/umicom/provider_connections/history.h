/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/history.h
 * PURPOSE: Keep bounded local exchanges and compose only explicitly selected history excerpts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CONNECTIONS_HISTORY_H
#define UMICOM_PROVIDER_CONNECTIONS_HISTORY_H
#include "umicom/provider_connections/chat.h"
#include "umicom/ai/types.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROVIDER_CHAT_HISTORY_CAPACITY 8U
#define UMI_PROVIDER_CHAT_HISTORY_REQUEST_CAPACITY 12352U
#define UMI_PROVIDER_CHAT_HISTORY_EXCERPTS 16U
    typedef struct UmiProviderChatHistory UmiProviderChatHistory;
    typedef struct UmiProviderChatHistoryEntry
    {
        uint64_t id, connection_revision;
        uint32_t output_limit;
        bool refused, truncated;
        char application[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
        char profile[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
        char connection[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
        char endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY];
        char requested_model[UMI_PROVIDER_CONNECTION_MODEL_CAPACITY];
        char reported_model[513];
        char request[UMI_PROVIDER_CHAT_HISTORY_REQUEST_CAPACITY];
        char reply[UMI_PROVIDER_CHAT_TEXT_CAPACITY];
    } UmiProviderChatHistoryEntry;
    typedef struct UmiProviderChatHistoryExcerpt
    {
        uint64_t entry_id;
        UmiAiRole role; /* USER chooses request; ASSISTANT chooses reply. */
        size_t byte_offset, byte_count;
    } UmiProviderChatHistoryExcerpt;

    /* One owner thread serializes this in-memory history. No disk, provider,
 * credential, automatic summarization or automatic resend is involved.
 * Unlike the existing small-message conversation ABI, these records can hold
 * complete reviewed requests and replies without truncating either one. */
    UmiStatus UmiProviderChatHistoryCreate(UmiProviderChatHistory **out);
    void UmiProviderChatHistoryDestroy(UmiProviderChatHistory *history);
    size_t UmiProviderChatHistoryCount(const UmiProviderChatHistory *history);
    uint64_t UmiProviderChatHistoryRevision(const UmiProviderChatHistory *history);
    /* Borrowed read-only entry, valid until the next successful mutation or
 * destruction. Copy text needed beyond that boundary; never modify the entry. */
    const UmiProviderChatHistoryEntry *UmiProviderChatHistoryAt(const UmiProviderChatHistory *history,
                                                                size_t index);
    /* Call only after a successful validated Run. This validates HTTP 200, text
 * bounds and UTF-8, but cannot prove that a caller's result came from a server.
 * A full history returns CAPACITY_EXCEEDED without evicting earlier entries.
 * Failure leaves history and out_id unchanged; it must never trigger a resend.
 * Authentication credentials, credential references and raw transport bodies
 * are never copied into entries. Prompt/reply content is retained as supplied;
 * this owner does not redact secrets that a user includes in their own text. */
    UmiStatus UmiProviderChatHistoryAppend(UmiProviderChatHistory *history, const UmiProviderChatPlan *plan,
                                           const UmiProviderChatResult *result, uint64_t *out_id);
    /* Stable IDs are not reused, including after Clear. Stale mutations return
 * BUSY and do not affect the current history. Removed owned bytes are cleared. */
    UmiStatus UmiProviderChatHistoryRemove(UmiProviderChatHistory *history, uint64_t expected_revision,
                                           uint64_t id);
    UmiStatus UmiProviderChatHistoryClear(UmiProviderChatHistory *history, uint64_t expected_revision);
    /* Compose the selected byte ranges in caller order as quoted context, not
 * privileged system/tool messages. Ranges must be nonempty and align with
 * complete UTF-8 characters. Labels retain refusal/incomplete-reply meaning.
 * No unselected text, metadata or separators from earlier context are added.
 * The result must fit the ordinary context limit including labels; it is not
 * silently truncated. On any failure out_text is unchanged. Review the full
 * new request and obtain fresh approval before sending this copied context. */
    UmiStatus UmiProviderChatHistoryCompose(const UmiProviderChatHistory *history, uint64_t expected_revision,
                                            const UmiProviderChatHistoryExcerpt *excerpts, size_t count,
                                            char *out_text, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
