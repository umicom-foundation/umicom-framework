/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/history.c
 *
 * PURPOSE:
 *   Implement bounded, secret-free API execution history and filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/history.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t query_length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    query_length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < query_length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == query_length) return 1;
    }
    return 0;
}

/*
 * Initialise web workbench history from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_history_init(UmiWebWorkbenchHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    memset(history, 0, sizeof(*history));
    history->next_execution_id = 1U;
    history->revision = 1U;
}

/*
 * Provide the web workbench history begin operation used by this module and its client
 * applications.
 */
uint64_t umi_web_workbench_history_begin(
    UmiWebWorkbenchHistory *history,
    const UmiWebWorkbenchRequest *request)
{
    UmiWebWorkbenchHistoryEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || request == NULL) return 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (history->entry_count == UMI_WEB_WORKBENCH_MAX_HISTORY) {
        memmove(&history->entries[0], &history->entries[1],
            (history->entry_count - 1U) * sizeof(history->entries[0]));
        history->entry_count--;
    }
    entry = &history->entries[history->entry_count++];
    memset(entry, 0, sizeof(*entry));
    entry->execution_id = history->next_execution_id++;
    (void)umi_web_workbench_copy_text(entry->request_id,
        sizeof(entry->request_id), request->request_id);
    (void)umi_web_workbench_copy_text(entry->request_name,
        sizeof(entry->request_name), request->name);
    (void)umi_web_workbench_copy_text(entry->url, sizeof(entry->url), request->url);
    entry->method = request->method;
    entry->phase = UMI_WEB_WORKBENCH_RUNNING;
    entry->sent_bytes = request->body_length;
    history->revision++;
    return entry->execution_id;
}

/*
 * Provide the web workbench history complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_history_complete(
    UmiWebWorkbenchHistory *history,
    uint64_t execution_id,
    const UmiWebWorkbenchResponse *response)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || execution_id == 0U || response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->entry_count; ++index) {
        UmiWebWorkbenchHistoryEntry *entry = &history->entries[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (entry->execution_id == execution_id) {
            entry->phase = response->phase;
            entry->status = response->status;
            entry->http_status = response->http_status;
            entry->duration_us = response->timing.total_us;
            entry->received_bytes = response->received_bytes;
            entry->tls_verified = response->tls_verified;
            history->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find web workbench history while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWebWorkbenchHistoryEntry *umi_web_workbench_history_at(
    const UmiWebWorkbenchHistory *history,
    size_t newest_first_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || newest_first_index >= history->entry_count) return NULL;
    return &history->entries[history->entry_count - newest_first_index - 1U];
}

/*
 * Provide the web workbench history query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_history_query(
    const UmiWebWorkbenchHistory *history,
    const char *text,
    int minimum_http_status,
    UmiWebWorkbenchHistoryEntry *out_entries,
    size_t capacity)
{
    size_t offset;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || (out_entries == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (offset = 0U; offset < history->entry_count && count < capacity; ++offset) {
        const UmiWebWorkbenchHistoryEntry *entry =
            &history->entries[history->entry_count - offset - 1U];
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (minimum_http_status > 0 && entry->http_status < minimum_http_status) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_case_insensitive(entry->request_name, text) &&
            !contains_case_insensitive(entry->url, text)) continue;
        out_entries[count++] = *entry;
    }
    return count;
}

/*
 * Release or reset state held by web workbench history so the same storage can be reused
 * safely.
 */
void umi_web_workbench_history_clear(UmiWebWorkbenchHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    memset(history->entries, 0, sizeof(history->entries));
    history->entry_count = 0U;
    history->revision++;
}
