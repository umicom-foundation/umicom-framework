/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_library_history.c
 * PURPOSE: Stage full library recovery evidence before native publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workspace_library_history.h"
#include <stdlib.h>
#include <string.h>

/* Each entry is at most two bounded library archives. Eight entries use less
 * than eight MiB of retained text; staging one change is bounded separately. */
typedef struct LibraryHistoryEntry {
    char *before;
    char *after;
    size_t before_size;
    size_t after_size;
} LibraryHistoryEntry;
struct UmiUiWorkspaceLibraryHistory {
    LibraryHistoryEntry entries[UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT];
    size_t count;
    size_t cursor;
    uint64_t expected_revision;
    char application_id[64];
    char workspace_id[64];
    char layout_prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    bool bound;
    bool busy;
};

/* Clear moved slots too: each archive always has exactly one owning entry. */
static void entry_release(LibraryHistoryEntry *entry)
{
    free(entry->before); free(entry->after);
    memset(entry, 0, sizeof(*entry));
}

UmiStatus umi_ui_workspace_library_history_create(UmiUiWorkspaceLibraryHistory **out_history)
{
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiUiWorkspaceLibraryHistory *history = calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_ui_workspace_library_history_destroy(UmiUiWorkspaceLibraryHistory *history)
{
    if (history == NULL) return;
    for (size_t index = 0U; index < history->count; ++index) entry_release(&history->entries[index]);
    free(history);
}

UmiStatus umi_ui_workspace_library_history_read(const UmiUiWorkspaceLibraryHistory *history,
    uint64_t current_revision, UmiUiWorkspaceLibraryHistoryState *out_state)
{
    if (history == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiUiWorkspaceLibraryHistoryState state = {
        history->cursor, history->count - history->cursor, history->expected_revision,
        history->count != 0U && history->expected_revision != current_revision, history->busy};
    *out_state = state;
    return UMI_STATUS_OK;
}

/* Canonical export validates identifiers and layout bounds before we copy
 * scope strings or retain bytes. Measurement and encoding share one grammar. */
static UmiStatus archive_copy(const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, char **out_bytes, size_t *out_size)
{
    size_t size = 0U;
    UmiStatus status = umi_ui_workspace_library_export(scope, model, 0U, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) return status;
    if (size >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *bytes = malloc(size + 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_workspace_library_export(scope, model, 0U, bytes, size + 1U, &size);
    if (status != UMI_STATUS_OK) { free(bytes); return status; }
    *out_bytes = bytes; *out_size = size;
    return UMI_STATUS_OK;
}

/* This helper is called only after canonical export has validated scope. */
static bool same_scope(const UmiUiWorkspaceLibraryHistory *history,
    const UmiUiWorkspaceCheckpointScope *scope)
{
    return !history->bound || (strcmp(history->application_id, scope->application_id) == 0 &&
        strcmp(history->workspace_id, scope->workspace_id) == 0 &&
        strcmp(history->layout_prefix, scope->layout_prefix) == 0);
}

UmiStatus umi_ui_workspace_library_history_record(UmiUiWorkspaceLibraryHistory *history,
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *before, const UmiUiWorkspaceCustomisation *candidate,
    UmiUiWorkspaceLibraryHistoryPublisher publish, void *context)
{
    if (history == NULL || scope == NULL || before == NULL || candidate == NULL || publish == NULL ||
        before == candidate) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->busy || before->edit_active || candidate->edit_active) return UMI_STATUS_BUSY;
    if (candidate->revision <= before->revision) return UMI_STATUS_INVALID_STATE;
    LibraryHistoryEntry entry = {0};
    UmiStatus status = archive_copy(scope, before, &entry.before, &entry.before_size);
    if (status == UMI_STATUS_OK && !same_scope(history, scope)) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK) status = archive_copy(scope, candidate, &entry.after, &entry.after_size);
    if (status != UMI_STATUS_OK) { entry_release(&entry); return status; }
    /* Capture revision and scope before the callback can publish into before.
     * Successful publication is followed only by infallible ownership moves. */
    const bool stale = history->count != 0U && history->expected_revision != before->revision;
    const uint64_t published_revision = candidate->revision;
    char application_id[64], workspace_id[64], prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    memcpy(application_id, scope->application_id, strlen(scope->application_id) + 1U);
    memcpy(workspace_id, scope->workspace_id, strlen(scope->workspace_id) + 1U);
    memcpy(prefix, scope->layout_prefix, strlen(scope->layout_prefix) + 1U);
    history->busy = true;
    status = publish(candidate, context);
    history->busy = false;
    if (status != UMI_STATUS_OK) { entry_release(&entry); return status; }
    if (stale) history->cursor = 0U;
    for (size_t index = history->cursor; index < history->count; ++index) entry_release(&history->entries[index]);
    history->count = history->cursor;
    if (history->count == UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT) {
        entry_release(&history->entries[0]);
        memmove(history->entries, history->entries + 1U, (history->count - 1U) * sizeof(history->entries[0]));
        --history->count;
        memset(&history->entries[history->count], 0, sizeof(history->entries[0]));
    }
    history->entries[history->count++] = entry;
    history->cursor = history->count;
    history->expected_revision = published_revision;
    memcpy(history->application_id, application_id, strlen(application_id) + 1U);
    memcpy(history->workspace_id, workspace_id, strlen(workspace_id) + 1U);
    memcpy(history->layout_prefix, prefix, strlen(prefix) + 1U);
    history->bound = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_library_history_navigate(UmiUiWorkspaceLibraryHistory *history,
    const UmiUiWorkspaceCheckpointScope *scope, const UmiUiWorkspaceCustomisation *model,
    UmiUiWorkspaceLibraryHistoryDirection direction, uint64_t expected_revision,
    UmiUiWorkspaceLibraryHistoryPublisher publish, void *context)
{
    if (history == NULL || scope == NULL || model == NULL || publish == NULL ||
        (direction != UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO && direction != UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->busy || model->edit_active) return UMI_STATUS_BUSY;
    if (expected_revision != model->revision || (history->count != 0U &&
        history->expected_revision != model->revision)) return UMI_STATUS_INVALID_STATE;
    const bool undo = direction == UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO;
    if ((undo && history->cursor == 0U) || (!undo && history->cursor == history->count)) return UMI_STATUS_NOT_FOUND;
    /* Validate scope before comparing borrowed strings. This also refuses
     * a now-invalid host catalogue before attempting native publication. */
    size_t current_size = 0U;
    UmiStatus status = umi_ui_workspace_library_export(scope, model, 0U, NULL, 0U, &current_size);
    if (status != UMI_STATUS_OK) return status;
    if (!same_scope(history, scope)) return UMI_STATUS_PERMISSION_DENIED;
    const LibraryHistoryEntry *entry = &history->entries[undo ? history->cursor - 1U : history->cursor];
    UmiUiWorkspaceLibraryImport *review = NULL;
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_workspace_library_import_review(scope, model, undo ? entry->before : entry->after,
        undo ? entry->before_size : entry->after_size, &review);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_library_import_candidate(review, scope, model, candidate);
    if (status == UMI_STATUS_OK) {
        const uint64_t published_revision = candidate->revision;
        history->busy = true;
        status = publish(candidate, context);
        history->busy = false;
        if (status == UMI_STATUS_OK) {
            if (undo) --history->cursor; else ++history->cursor;
            history->expected_revision = published_revision;
        }
    }
    umi_ui_workspace_library_import_destroy(review);
    free(candidate);
    return status;
}
