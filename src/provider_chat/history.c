/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/history.c
 * PURPOSE: Own complete local exchanges with stable IDs and atomic bounded mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "history_internal.h"
#include "internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_document.h"
#include <stdlib.h>
#include <string.h>
_Static_assert(UMI_PROVIDER_CHAT_HISTORY_REQUEST_CAPACITY >= UMI_PROVIDER_CHAT_INPUT_CAPACITY,
               "History must hold the complete reviewed message");
_Static_assert(sizeof(((UmiProviderChatHistoryEntry *)0)->reported_model) >=
                   sizeof(((UmiProviderChatResult *)0)->model),
               "History must hold the complete reported model identifier");

/* Reuse the Framework JSON string validator for UTF-8 rather than growing a
 * second decoder here. Only tab, CR and LF are allowed among ASCII controls,
 * so twice the input capacity plus quotes bounds the escaped representation. */
UmiStatus UmiProviderChatHistoryText(const char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U || capacity > UMI_PROVIDER_CHAT_TEXT_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < capacity && text[length] != '\0')
    {
        unsigned char c = (unsigned char)text[length++];
        if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length == capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t bound = capacity * 2U + 3U;
    char *json = calloc(bound, 1U);
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof(*document));
    if (json == NULL || document == NULL)
    {
        free(json);
        free(document);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_ai_mcp_json_escape_string(text, json, bound);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageRuntimeJsonParseComplete(json, document);
    umi_secret_clear(json, bound);
    free(json);
    umi_secret_clear(document, sizeof(*document));
    free(document);
    return status;
}
UmiStatus UmiProviderChatHistoryCreate(UmiProviderChatHistory **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = calloc(1U, sizeof(**out));
    if (*out == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    (*out)->revision = 1U;
    (*out)->next_id = 1U;
    return UMI_STATUS_OK;
}
void UmiProviderChatHistoryDestroy(UmiProviderChatHistory *history)
{
    if (history != NULL)
    {
        umi_secret_clear(history, sizeof(*history));
        free(history);
    }
}
size_t UmiProviderChatHistoryCount(const UmiProviderChatHistory *history)
{
    return history != NULL ? history->count : 0U;
}
uint64_t UmiProviderChatHistoryRevision(const UmiProviderChatHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
const UmiProviderChatHistoryEntry *UmiProviderChatHistoryAt(const UmiProviderChatHistory *history,
                                                            size_t index)
{
    return history != NULL && index < history->count ? &history->entries[index] : NULL;
}
const UmiProviderChatHistoryEntry *UmiProviderChatHistoryFind(const UmiProviderChatHistory *history,
                                                              uint64_t id)
{
    if (history != NULL)
        for (size_t i = 0U; i < history->count; ++i)
            if (history->entries[i].id == id)
                return &history->entries[i];
    return NULL;
}
UmiStatus UmiProviderChatHistoryAppend(UmiProviderChatHistory *history, const UmiProviderChatPlan *plan,
                                       const UmiProviderChatResult *result, uint64_t *out_id)
{
    if (history == NULL || plan == NULL || result == NULL || out_id == NULL || result->http_status != 200U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->count >= UMI_PROVIDER_CHAT_HISTORY_CAPACITY || history->revision == UINT64_MAX ||
        history->next_id == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiProviderChatHistoryText(plan->input, sizeof(plan->input));
    if (status == UMI_STATUS_OK)
        status = UmiProviderChatHistoryText(result->text, sizeof(result->text));
    if (status == UMI_STATUS_OK)
        status = UmiProviderChatHistoryText(result->model, sizeof(result->model));
    if (status != UMI_STATUS_OK)
        return status;
    /* Validate everything before claiming the next slot. A failed append must
     * not create a request without its reply or advance the user's selection. */
    UmiProviderChatHistoryEntry *entry = &history->entries[history->count];
    const UmiProviderConnectionCheckPlan *connection = UmiProviderChatConnection(plan);
    entry->id = history->next_id++;
    entry->connection_revision = connection->revision;
    entry->output_limit = UmiProviderChatOutputLimit(plan);
    entry->refused = result->refused;
    entry->truncated = result->truncated;
    strcpy(entry->application, connection->application);
    strcpy(entry->profile, connection->profile);
    strcpy(entry->connection, connection->connection.id);
    strcpy(entry->endpoint, connection->connection.endpoint);
    strcpy(entry->requested_model, connection->connection.model);
    strcpy(entry->reported_model, result->model);
    strcpy(entry->request, UmiProviderChatInput(plan));
    strcpy(entry->reply, result->text);
    ++history->count;
    ++history->revision;
    *out_id = entry->id;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderChatHistoryRemove(UmiProviderChatHistory *history, uint64_t expected_revision,
                                       uint64_t id)
{
    if (history == NULL || id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->revision != expected_revision)
        return UMI_STATUS_BUSY;
    if (history->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t index = 0U;
    while (index < history->count && history->entries[index].id != id)
        ++index;
    if (index == history->count)
        return UMI_STATUS_NOT_FOUND;
    /* Compact owned entries, then clear the unused tail so no removed copy
     * survives in capacity reserved for the next exchange. */
    umi_secret_clear(&history->entries[index], sizeof(history->entries[index]));
    if (index + 1U < history->count)
        memmove(&history->entries[index], &history->entries[index + 1U],
                (history->count - index - 1U) * sizeof(history->entries[0]));
    --history->count;
    umi_secret_clear(&history->entries[history->count], sizeof(history->entries[0]));
    ++history->revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderChatHistoryClear(UmiProviderChatHistory *history, uint64_t expected_revision)
{
    if (history == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->revision != expected_revision)
        return UMI_STATUS_BUSY;
    if (history->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_secret_clear(history->entries, sizeof(history->entries));
    history->count = 0U;
    ++history->revision;
    return UMI_STATUS_OK;
}
