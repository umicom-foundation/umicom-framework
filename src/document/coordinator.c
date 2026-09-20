/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/coordinator.c
 *
 * PURPOSE:
 *   Coordinate decoded working copies, toolkit-neutral views, undo/redo,
 *   navigation, atomic saves and external-change protection in one service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/coordinator.h"
#include "umicom/document/edit.h"
#include "umicom/document/text_encoding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/document/file_identity.h"
#include "umicom/document/fingerprint.h"
#include "umicom/document/language_detection.h"
#include "umicom/document/line_endings.h"
#include "umicom/document/loader.h"
#include "umicom/document/local_provider.h"
#include "umicom/document/saver.h"
#include "umicom/document/uri.h"
#include "umicom/editor/search_engine.h"

typedef struct UmiDocumentCoordinatorEntry {
    UmiDocumentId document_id;
    char view_id[UMI_UI_ID_CAPACITY];
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding line_ending;
    UmiDocumentConflictState conflict;
    UmiDocumentFingerprint baseline;
    char *undo[UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY];
    size_t undo_count;
    char *redo[UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY];
    size_t redo_count;
    int pristine_virtual;
    /* Cache only the comparison needed by menu availability. Selection/focus
     * refreshes must not repeatedly copy an eight-megabyte document. */
    uint64_t edit_text_revision;
    uint64_t edit_store_revision;
    int edit_pending;
    int edit_cache_valid;
} UmiDocumentCoordinatorEntry;

struct UmiDocumentCoordinator {
    UmiDocumentStore *store;
    UmiUiWorkbench *workbench;
    UmiDocumentProvider provider;
    UmiDocumentCoordinatorEntry entries[UMI_DOCUMENT_MAX_WORKING_COPIES];
    size_t count;
    uint64_t next_untitled;
};

/* Inspection is a provider read; it must never bypass that provider with a
 * local fopen. Publish conflict state only for a conclusive comparison. */
static UmiStatus CheckExternalIndex(UmiDocumentCoordinator *coordinator,
    size_t index, int *outChanged)
{
    UmiDocumentSnapshot snapshot;
    UmiDocumentFingerprint current = {0};
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    *outChanged = 0;
    UmiStatus status = umi_document_store_snapshot(coordinator->store,
        entry->document_id, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.has_path || !entry->baseline.valid) return UMI_STATUS_OK;
    UmiDocumentFingerprint baseline = entry->baseline;
    char viewId[UMI_UI_ID_CAPACITY];
    (void)snprintf(viewId, sizeof viewId, "%s", entry->view_id);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentTextInfo expectedText, latestText;
    status = UmiUiDocumentViewModelTextInfo(views, viewId, &expectedText);
    if (status != UMI_STATUS_OK) return status;
    status = UmiDocumentFingerprintRead(&coordinator->provider, snapshot.path,
        UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES, &current);
    /* A provider callback may close or rename a document. Never publish its
     * old comparison into the entry that has since taken the same slot. */
    size_t currentIndex = SIZE_MAX;
    for (size_t candidate = 0U; candidate < coordinator->count; ++candidate) {
        if (coordinator->entries[candidate].document_id == snapshot.document_id &&
            strcmp(coordinator->entries[candidate].view_id, viewId) == 0) {
            currentIndex = candidate;
            break;
        }
    }
    if (currentIndex == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiDocumentSnapshot latest;
    UmiStatus latestStatus = umi_document_store_snapshot(coordinator->store,
        snapshot.document_id, &latest);
    if (latestStatus != UMI_STATUS_OK) return latestStatus;
    latestStatus = UmiUiDocumentViewModelTextInfo(views, viewId, &latestText);
    if (latestStatus != UMI_STATUS_OK) return latestStatus;
    if (latestText.text_revision != expectedText.text_revision)
        return UMI_STATUS_INVALID_STATE;
    entry = &coordinator->entries[currentIndex];
    if (latest.revision != snapshot.revision ||
        latest.saved_revision != snapshot.saved_revision || !latest.has_path ||
        strcmp(latest.path, snapshot.path) != 0 ||
        !umi_document_fingerprint_equal(&entry->baseline, &baseline))
        return UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_NOT_FOUND) {
        entry->conflict = UMI_DOCUMENT_CONFLICT_DELETED_EXTERNALLY;
        *outChanged = 1;
        (void)umi_document_store_mark_external_change(coordinator->store,
            entry->document_id, 1);
    } else if (status == UMI_STATUS_OK) {
        *outChanged = !umi_document_fingerprint_equal(&entry->baseline, &current);
        entry->conflict = *outChanged ? UMI_DOCUMENT_CONFLICT_EXTERNAL_CHANGE
                                     : UMI_DOCUMENT_CONFLICT_NONE;
        status = umi_document_store_mark_external_change(coordinator->store,
            entry->document_id, *outChanged);
    }
    return status;
}

/* Release or reset state held by history so the same storage can be reused safely. */
static void history_clear(char **items, size_t *count)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < *count; ++index) free(items[index]);
    *count = 0U;
}

static void HistoryPushOwned(char **items, size_t *count, char *text);

/* Provide the history push operation used by this module and its client applications. */
static UmiStatus history_push(char **items, size_t *count, const char *text)
{
    char *copy;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || count == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(copy, text, length + 1U);
    HistoryPushOwned(items, count, copy);
    return UMI_STATUS_OK;
}

/* History takes ownership only after an edit has committed successfully. */
static void HistoryPushOwned(char **items, size_t *count, char *text)
{
    size_t incoming = strlen(text) + 1U;
    size_t bytes = 0U;
    for (size_t index = 0U; index < *count; ++index)
        bytes += strlen(items[index]) + 1U;
    while (*count != 0U && (*count == UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY ||
            incoming > UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET - bytes)) {
        bytes -= strlen(items[0]) + 1U;
        free(items[0]);
        memmove(items, items + 1U, (*count - 1U) * sizeof *items);
        --*count;
    }
    items[(*count)++] = text;
}

/* Provide the history pop operation used by this module and its client applications. */
static char *history_pop(char **items, size_t *count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL || count == NULL || *count == 0U) return NULL;
    *count -= 1U;
    return items[*count];
}

/* Provide the find view operation used by this module and its client applications. */
static size_t find_view(const UmiDocumentCoordinator *coordinator,
                        const char *view_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < coordinator->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(coordinator->entries[index].view_id, view_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the active index operation used by this module and its client applications. */
static size_t active_index(const UmiDocumentCoordinator *coordinator)
{
    UmiUiWorkbenchSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL ||
        umi_ui_workbench_snapshot(coordinator->workbench, &snapshot) != UMI_STATUS_OK) {
        return SIZE_MAX;
    }
    return find_view(coordinator, snapshot.active_document_view);
}

/* Provide the copy store text operation used by this module and its client applications. */
static UmiStatus copy_store_text(UmiDocumentCoordinator *coordinator,
                                 size_t index,
                                 char **out_text,
                                 size_t *out_length)
{
    return umi_document_store_copy_text(coordinator->store,
                                        coordinator->entries[index].document_id,
                                        out_text,
                                        out_length);
}

/* Release or reset state held by entry so the same storage can be reused safely. */
static void entry_dispose(UmiDocumentCoordinatorEntry *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    history_clear(entry->undo, &entry->undo_count);
    history_clear(entry->redo, &entry->redo_count);
    (void)memset(entry, 0, sizeof(*entry));
}

/* Provide the refresh view operation used by this module and its client applications. */
static UmiStatus refresh_view(UmiDocumentCoordinator *coordinator, size_t index)
{
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiDocumentSnapshot store_snapshot;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentLanguageIdentity language;
    char *text = NULL;
    size_t length = 0U;
    UmiStatus status;
    int existingView;
    status = umi_document_store_snapshot(coordinator->store,
                                         entry->document_id,
                                         &store_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_store_text(coordinator, index, &text, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    existingView = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench), entry->view_id, &view) == UMI_STATUS_OK;
    if (!existingView) (void)memset(&view, 0, sizeof(view));
    (void)snprintf(view.view_id, sizeof(view.view_id), "%s", entry->view_id);
    (void)snprintf(view.document_id, sizeof(view.document_id),
                   "document.%llu", (unsigned long long)entry->document_id);
    (void)snprintf(view.title, sizeof(view.title), "%s", store_snapshot.display_name);
    view.dirty = store_snapshot.dirty || !store_snapshot.has_path;
    if (!existingView) {
        view.active = 1;
        view.pinned = 1;
    }
    if (view.cursor_offset > length) view.cursor_offset = length;
    if (view.selection_length > length - view.cursor_offset)
        view.selection_length = length - view.cursor_offset;
    /* Apply this branch only when its contract condition is satisfied. */
    if (store_snapshot.has_path) {
        (void)umi_document_uri_from_path(store_snapshot.path, view.uri, sizeof(view.uri));
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)snprintf(view.uri, sizeof(view.uri), "untitled:///%s",
                       store_snapshot.display_name);
    }
    (void)umi_document_language_detect(store_snapshot.display_name, &language);
    (void)snprintf(view.language_id, sizeof(view.language_id), "%s", language.language_id);
    (void)snprintf(view.icon_name, sizeof(view.icon_name), "%.*s",
                   (int)sizeof(view.icon_name) - 1, language.icon_name);
    status = UmiUiDocumentViewModelUpsertText(
        umi_ui_workbench_documents(coordinator->workbench), &view, text, length);
    umi_document_store_free_text(text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !existingView) {
        status = umi_ui_workbench_activate_document(coordinator->workbench,
                                                    entry->view_id);
    }
    return status;
}

/* Provide the register entry operation used by this module and its client applications. */
static UmiStatus register_entry(UmiDocumentCoordinator *coordinator,
                                UmiDocumentId document_id,
                                UmiDocumentTextEncoding encoding,
                                UmiDocumentLineEnding line_ending,
                                const UmiDocumentFingerprint *baseline,
                                const char *preferred_view_id,
                                size_t *out_index)
{
    UmiDocumentCoordinatorEntry *entry;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &coordinator->entries[coordinator->count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->document_id = document_id;
    entry->encoding = encoding;
    entry->line_ending = line_ending == UMI_DOCUMENT_LINE_ENDING_NONE ||
                         line_ending == UMI_DOCUMENT_LINE_ENDING_MIXED
        ? UMI_DOCUMENT_LINE_ENDING_LF : line_ending;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (baseline != NULL) entry->baseline = *baseline;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preferred_view_id != NULL && preferred_view_id[0] != '\0') {
        (void)snprintf(entry->view_id, sizeof(entry->view_id), "%s", preferred_view_id);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)snprintf(entry->view_id, sizeof(entry->view_id),
                       "umicom.document.view.%llu", (unsigned long long)document_id);
    }
    *out_index = coordinator->count++;
    return UMI_STATUS_OK;
}

/*
 * Provide the import existing views operation used by this module and its client
 * applications.
 */
static UmiStatus import_existing_views(UmiDocumentCoordinator *coordinator)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    size_t view_count = umi_ui_document_view_model_count(views);
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < view_count; ++index) {
        UmiUiDocumentViewSnapshot view;
        UmiDocumentId document_id;
        size_t entry_index;
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_document_view_model_at(views, index, &view) != UMI_STATUS_OK) continue;
        status = umi_document_store_new(coordinator->store,
                                        view.title[0] != '\0' ? view.title : "Untitled",
                                        &document_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        char *importedText = NULL;
        size_t importedLength = 0U;
        status = UmiUiDocumentViewModelCopyText(views, view.view_id, &importedText, &importedLength);
        if (status == UMI_STATUS_OK)
            status = umi_document_store_replace_text(coordinator->store,
                document_id, importedText, importedLength);
        UmiUiDocumentViewModelFreeText(importedText);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)umi_document_store_close(coordinator->store, document_id, 1);
            return status;
        }
        status = register_entry(coordinator, document_id,
                                UMI_DOCUMENT_ENCODING_UTF8,
                                UMI_DOCUMENT_LINE_ENDING_LF,
                                NULL, view.view_id, &entry_index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)umi_document_store_close(coordinator->store, document_id, 1);
            return status;
        }
        /* Built-in umicom:// welcome/reference text is not an unsaved user
         * file until the learner actually edits it. Restored dirty drafts are
         * never exempted from saving. */
        coordinator->entries[entry_index].pristine_virtual =
            !view.dirty && strncmp(view.uri, "umicom://", 9U) == 0;
        (void)umi_ui_document_view_model_upsert(views, &view);
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise document coordinator from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_document_coordinator_create(
    UmiDocumentStore *store,
    UmiUiWorkbench *workbench,
    const UmiDocumentProvider *provider,
    UmiDocumentCoordinator **out_coordinator)
{
    UmiDocumentCoordinator *coordinator;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || workbench == NULL || out_coordinator == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_coordinator = NULL;
    coordinator = (UmiDocumentCoordinator *)calloc(1U, sizeof(*coordinator));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    coordinator->store = store;
    coordinator->workbench = workbench;
    coordinator->provider = provider != NULL ? *provider : umi_document_local_provider();
    coordinator->next_untitled = 1U;
    status = umi_document_provider_validate(&coordinator->provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = import_existing_views(coordinator);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /* A failed import owns only the records it just created. Existing
         * caller records and contributed views remain available for retry. */
        for (size_t index = 0U; index < coordinator->count; ++index)
            (void)umi_document_store_close(store, coordinator->entries[index].document_id, 1);
        umi_document_coordinator_destroy(coordinator);
        return status;
    }
    *out_coordinator = coordinator;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by document coordinator so the same storage can be reused
 * safely.
 */
void umi_document_coordinator_destroy(UmiDocumentCoordinator *coordinator)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < coordinator->count; ++index) {
        entry_dispose(&coordinator->entries[index]);
    }
    free(coordinator);
}

/* Finish the store/view attachment as one operation. Failure never leaves an
 * invisible working copy which consumes a slot on the next Open attempt. */
static UmiStatus FinishNewDocument(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, size_t index, UmiStatus status,
    char *outViewId, size_t capacity)
{
    int attachAttempted = 0;
    if (status == UMI_STATUS_OK && outViewId != NULL &&
        strlen(coordinator->entries[index].view_id) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        UmiUiDocumentViewSnapshot existing;
        UmiStatus lookup = umi_ui_document_view_model_find(
            umi_ui_workbench_documents(coordinator->workbench),
            coordinator->entries[index].view_id, &existing);
        /* A contributed view must never be overwritten by a generated ID. */
        if (lookup == UMI_STATUS_OK) status = UMI_STATUS_ALREADY_EXISTS;
        else if (lookup != UMI_STATUS_NOT_FOUND) status = lookup;
    }
    if (status == UMI_STATUS_OK) {
        attachAttempted = 1;
        status = refresh_view(coordinator, index);
    }
    if (status == UMI_STATUS_OK) {
        if (outViewId != NULL) strcpy(outViewId, coordinator->entries[index].view_id);
        return status;
    }
    if (index != SIZE_MAX) {
        if (attachAttempted)
            (void)umi_ui_document_view_model_remove(umi_ui_workbench_documents(coordinator->workbench),
                coordinator->entries[index].view_id);
        entry_dispose(&coordinator->entries[index]);
        --coordinator->count; /* Newly registered entries are always last. */
    }
    if (documentId != 0U) (void)umi_document_store_close(coordinator->store, documentId, 1);
    return status;
}

/*
 * Provide the document coordinator new operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_new(UmiDocumentCoordinator *coordinator,
                                       const char *suggested_name,
                                       char *out_view_id,
                                       size_t capacity)
{
    char name[UMI_DOCUMENT_NAME_CAPACITY];
    UmiDocumentId document_id = 0U;
    size_t index = SIZE_MAX;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || (out_view_id != NULL && capacity == 0U) ||
        (out_view_id == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (out_view_id != NULL) out_view_id[0] = '\0';
    if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES ||
        umi_ui_document_view_model_count(umi_ui_workbench_documents(coordinator->workbench)) >= UMI_UI_DOCUMENT_VIEW_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (suggested_name != NULL && strlen(suggested_name) >= UMI_DOCUMENT_DISPLAY_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suggested_name != NULL && suggested_name[0] != '\0') {
        (void)snprintf(name, sizeof(name), "%s", suggested_name);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)snprintf(name, sizeof(name), "Untitled-%llu.c",
                       (unsigned long long)coordinator->next_untitled++);
    }
    status = umi_document_store_new(coordinator->store, name, &document_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = register_entry(coordinator, document_id,
                                UMI_DOCUMENT_ENCODING_UTF8,
                                UMI_DOCUMENT_LINE_ENDING_LF,
                                NULL, NULL, &index);
    }
    return FinishNewDocument(coordinator, document_id, index, status,
        out_view_id, capacity);
}

/*
 * Provide the document coordinator open operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_open(UmiDocumentCoordinator *coordinator,
                                        const char *path,
                                        char *out_view_id,
                                        size_t capacity)
{
    UmiDocumentLoadOptions options = umi_document_load_options_default();
    UmiDocumentLoadResult loaded;
    UmiDocumentId document_id = 0U;
    const char *name;
    const char *separator;
    size_t index = SIZE_MAX;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || path == NULL || path[0] == '\0' ||
        (out_view_id != NULL && capacity == 0U) || (out_view_id == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_view_id != NULL) out_view_id[0] = '\0';
    char normalised[UMI_PATH_CAPACITY];
    status = umi_path_normalise(path, normalised, sizeof normalised);
    if (status != UMI_STATUS_OK) return status;
    for (size_t existing = 0U; existing < coordinator->count; ++existing) {
        UmiDocumentSnapshot snapshot;
        status = umi_document_store_snapshot(coordinator->store,
            coordinator->entries[existing].document_id, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (snapshot.has_path && umi_path_equal(snapshot.path, normalised)) {
            const char *viewId = coordinator->entries[existing].view_id;
            if (out_view_id != NULL && strlen(viewId) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            status = umi_ui_workbench_activate_document(coordinator->workbench, viewId);
            if (status == UMI_STATUS_OK && out_view_id != NULL) strcpy(out_view_id, viewId);
            return status;
        }
    }
    if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES ||
        umi_ui_document_view_model_count(umi_ui_workbench_documents(coordinator->workbench)) >= UMI_UI_DOCUMENT_VIEW_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    options.maximum_bytes = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
    options.normalise_to = UMI_DOCUMENT_LINE_ENDING_LF;
    status = umi_document_load(&coordinator->provider, path, &options, &loaded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    separator = strrchr(path, '/');
#ifdef _WIN32
    {
        const char *backslash = strrchr(path, '\\');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (backslash != NULL && (separator == NULL || backslash > separator)) separator = backslash;
    }
#endif
    if (loaded.text_length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) {
        umi_document_load_result_dispose(&loaded);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    name = separator != NULL ? separator + 1U : path;
    status = umi_document_store_create_loaded(coordinator->store, name, path,
                                              loaded.text, loaded.text_length,
                                              &document_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = register_entry(coordinator, document_id,
                                loaded.detected_encoding,
                                loaded.detected_line_ending,
                                &loaded.fingerprint, NULL, &index);
    }
    status = FinishNewDocument(coordinator, document_id, index, status,
        out_view_id, capacity);
    umi_document_load_result_dispose(&loaded);
    return status;
}

/* Provide the sync index operation used by this module and its client applications. */
static UmiStatus sync_index(UmiDocumentCoordinator *coordinator, size_t index)
{
    UmiUiDocumentViewSnapshot view;
    char *stored = NULL;
    char *draft = NULL;
    size_t length = 0U;
    size_t draftLength = 0U;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_ui_document_view_model_find(views,
        coordinator->entries[index].view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    status = UmiUiDocumentViewModelCopyText(views, view.view_id, &draft, &draftLength);
    if (status != UMI_STATUS_OK) return status;
    status = copy_store_text(coordinator, index, &stored, &length);
    if (status == UMI_STATUS_OK &&
        (length != draftLength || memcmp(stored, draft, length) != 0)) {
        char *prepared[UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY] = {0};
        size_t preparedCount = 0U;
        /* Allocate before changing either the store or the existing history. */
        status = history_push(prepared, &preparedCount, stored);
        if (status == UMI_STATUS_OK)
            status = umi_document_store_replace_text(coordinator->store,
                coordinator->entries[index].document_id, draft, draftLength);
        if (status == UMI_STATUS_OK) {
            HistoryPushOwned(coordinator->entries[index].undo,
                &coordinator->entries[index].undo_count, history_pop(prepared, &preparedCount));
            history_clear(coordinator->entries[index].redo, &coordinator->entries[index].redo_count);
            coordinator->entries[index].pristine_virtual = 0;
        }
        history_clear(prepared, &preparedCount);
    } else if (status == UMI_STATUS_OK && view.dirty) {
        coordinator->entries[index].pristine_virtual = 0;
    }
    umi_document_store_free_text(stored);
    UmiUiDocumentViewModelFreeText(draft);
    return status;
}

/*
 * Provide the document coordinator sync active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_sync_active(
    UmiDocumentCoordinator *coordinator)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    return index == SIZE_MAX ? UMI_STATUS_NOT_FOUND : sync_index(coordinator, index);
}

/* A Save As destination cannot also be owned by another open working copy.
 * This checks normalised path identity, not symlink or hard-link aliases. */
static UmiStatus CheckSaveDestination(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, const char *path)
{
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(path, normalised, sizeof normalised);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < umi_document_store_count(coordinator->store); ++index) {
        UmiDocumentSnapshot snapshot;
        status = umi_document_store_at(coordinator->store, index, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (snapshot.document_id != documentId && snapshot.has_path &&
            umi_path_equal(snapshot.path, normalised)) return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}

/* Provide the save index as operation used by this module and its client applications. */
static UmiStatus save_index_as(UmiDocumentCoordinator *coordinator,
                               size_t index,
                               const char *path)
{
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiDocumentSaveOptions options = umi_document_save_options_default();
    UmiDocumentSaveResult result;
    char *text = NULL;
    size_t length = 0U;
    int changed = 0;
    UmiStatus status = CheckSaveDestination(coordinator, entry->document_id, path);
    if (status != UMI_STATUS_OK) return status;
    status = sync_index(coordinator, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this operation only while the related capability or state is available. */
    if (entry->baseline.valid) {
        UmiDocumentSnapshot snapshot;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_document_store_snapshot(coordinator->store, entry->document_id,
                                        &snapshot) == UMI_STATUS_OK &&
            snapshot.has_path && umi_path_equal(snapshot.path, path)) {
            UmiStatus check = CheckExternalIndex(coordinator, index, &changed);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((check == UMI_STATUS_OK && changed) || check == UMI_STATUS_NOT_FOUND) {
                entry->conflict = check == UMI_STATUS_NOT_FOUND
                    ? UMI_DOCUMENT_CONFLICT_DELETED_EXTERNALLY
                    : UMI_DOCUMENT_CONFLICT_EXTERNAL_CHANGE;
                (void)umi_document_store_mark_external_change(
                    coordinator->store, entry->document_id, 1);
                return UMI_STATUS_INVALID_STATE;
            }
            /* Failure to inspect the existing file is not permission to
             * overwrite it without the external-change check. */
            if (check != UMI_STATUS_OK) return check;
        }
    }
    status = copy_store_text(coordinator, index, &text, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    options.encoding = entry->encoding == UMI_DOCUMENT_ENCODING_UNKNOWN ||
                       entry->encoding == UMI_DOCUMENT_ENCODING_BINARY
        ? UMI_DOCUMENT_ENCODING_UTF8 : entry->encoding;
    options.include_bom = options.encoding == UMI_DOCUMENT_ENCODING_UTF8_BOM ||
                          options.encoding == UMI_DOCUMENT_ENCODING_UTF16_LE ||
                          options.encoding == UMI_DOCUMENT_ENCODING_UTF16_BE;
    options.line_ending = entry->line_ending;
    status = umi_document_saver_write(&coordinator->provider, path, text, length,
                                      &options, &result);
    umi_document_store_free_text(text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        entry->baseline = result.fingerprint;
        entry->conflict = UMI_DOCUMENT_CONFLICT_NONE;
        status = umi_document_store_mark_saved_as(coordinator->store,
                                                  entry->document_id, path);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = refresh_view(coordinator, index);
    return status;
}

/*
 * Provide the document coordinator save active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active(
    UmiDocumentCoordinator *coordinator)
{
    UmiDocumentSnapshot snapshot;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_document_store_snapshot(coordinator->store,
                                    coordinator->entries[index].document_id,
                                    &snapshot) != UMI_STATUS_OK) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!snapshot.has_path) return UMI_STATUS_INVALID_STATE;
    return save_index_as(coordinator, index, snapshot.path);
}

/* Save All uses the same working-copy and conflict boundary as Save. There is
 * no cross-file transaction: a later I/O failure preserves earlier successful
 * saves and reports their count. Unsaved names are rejected before any write. */
UmiStatus UmiDocumentCoordinatorSaveAll(UmiDocumentCoordinator *coordinator,
    size_t *outSaved)
{
    UmiDocumentSnapshot snapshot;
    UmiStatus status;
    if (outSaved != NULL) *outSaved = 0U;
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < coordinator->count; ++index) {
        status = sync_index(coordinator, index);
        if (status != UMI_STATUS_OK) return status;
        status = umi_document_store_snapshot(coordinator->store,
            coordinator->entries[index].document_id, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (coordinator->entries[index].pristine_virtual) continue;
        if (snapshot.dirty && !snapshot.has_path) return UMI_STATUS_INVALID_STATE;
    }
    for (size_t index = 0U; index < coordinator->count; ++index) {
        status = umi_document_store_snapshot(coordinator->store,
            coordinator->entries[index].document_id, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (coordinator->entries[index].pristine_virtual) continue;
        if (!snapshot.dirty) {
            /* Typing and then restoring identical text may leave only the
             * adapter's modified marker. Save All clears that marker too. */
            UmiUiDocumentViewSnapshot view;
            status = umi_ui_document_view_model_find(umi_ui_workbench_documents(coordinator->workbench),
                coordinator->entries[index].view_id, &view);
            if (status != UMI_STATUS_OK) return status;
            if (view.dirty) {
                view.dirty = 0;
                status = umi_ui_document_view_model_upsert(umi_ui_workbench_documents(coordinator->workbench), &view);
                if (status != UMI_STATUS_OK) return status;
            }
            continue;
        }
        status = save_index_as(coordinator, index, snapshot.path);
        if (status != UMI_STATUS_OK) return status;
        if (outSaved != NULL) ++*outSaved;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the document coordinator save active as operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active_as(
    UmiDocumentCoordinator *coordinator,
    const char *path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = active_index(coordinator);
    return index == SIZE_MAX ? UMI_STATUS_NOT_FOUND
                             : save_index_as(coordinator, index, path);
}

/* Resolve stable document identity directly. Changing the active tab just to
 * perform a save would create selection events and could target a newer tab. */
UmiStatus UmiDocumentCoordinatorSaveAs(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, const char *path)
{
    if (coordinator == NULL || documentId == 0U || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < coordinator->count; ++index) {
        if (coordinator->entries[index].document_id == documentId)
            return save_index_as(coordinator, index, path);
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the document coordinator close active operation used by this module and its
 * client applications.
 */
UmiStatus UmiDocumentCoordinatorClose(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, int force)
{
    size_t index;
    size_t move_count;
    UmiDocumentSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t previouslyActive = active_index(coordinator);
    index = SIZE_MAX;
    for (size_t candidate = 0U; candidate < coordinator->count; ++candidate) {
        if (coordinator->entries[candidate].document_id == documentId) { index = candidate; break; }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = sync_index(coordinator, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_document_store_snapshot(coordinator->store,
                                         coordinator->entries[index].document_id,
                                         &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!force && !coordinator->entries[index].pristine_virtual &&
        (snapshot.dirty || !snapshot.has_path)) return UMI_STATUS_INVALID_STATE;
    status = umi_document_store_close(coordinator->store,
                                      coordinator->entries[index].document_id,
                                      force || coordinator->entries[index].pristine_virtual);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_ui_document_view_model_remove(
        umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id);
    entry_dispose(&coordinator->entries[index]);
    move_count = coordinator->count - index - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (move_count > 0U) {
        (void)memmove(&coordinator->entries[index],
                      &coordinator->entries[index + 1U],
                      move_count * sizeof(coordinator->entries[0]));
    }
    coordinator->count -= 1U;
    (void)memset(&coordinator->entries[coordinator->count], 0,
                 sizeof(coordinator->entries[0]));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (coordinator->count > 0U && previouslyActive == index) {
        size_t activate = index < coordinator->count ? index : coordinator->count - 1U;
        return umi_ui_workbench_activate_document(coordinator->workbench,
                                                  coordinator->entries[activate].view_id);
    }
    return UMI_STATUS_OK;
}

/* Keep the existing active-document convenience operation on the same path. */
UmiStatus umi_document_coordinator_close_active(UmiDocumentCoordinator *coordinator,
    int force)
{
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiDocumentCoordinatorClose(coordinator,
        coordinator->entries[index].document_id, force);
}

/* Provide the apply history operation used by this module and its client applications. */
static UmiStatus apply_history(UmiDocumentCoordinator *coordinator,
                               int redo_direction, size_t index)
{
    /* Targeted history now receives its document entry from the caller.
     * The old active-tab lookup below is retained for migration reference.
     * Reintroducing it both redeclares the parameter and loses the document
     * chosen by UmiDocumentCoordinatorUndo / UmiDocumentCoordinatorRedo.
     * Active-document wrappers still call active_index before this helper.
     */
    // size_t index = active_index(coordinator);
    UmiDocumentCoordinatorEntry *entry;
    UmiUiDocumentViewSnapshot view;
    char *current = NULL;
    size_t current_length = 0U;
    char *target;
    UmiStatus status;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_document_view_model_find(umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    if (view.read_only) return UMI_STATUS_PERMISSION_DENIED;
    status = sync_index(coordinator, index);
    if (status != UMI_STATUS_OK) return status;
    entry = &coordinator->entries[index];
    size_t count = redo_direction ? entry->redo_count : entry->undo_count;
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    target = redo_direction ? entry->redo[count - 1U] : entry->undo[count - 1U];
    /* Reserve the full projection first. A failed allocation leaves the store
     * and both history stacks unchanged. This runs on the document owner. */
    size_t targetLength = strlen(target);
    status = UmiUiDocumentViewModelReserveText(umi_ui_workbench_documents(coordinator->workbench),
        view.view_id, targetLength);
    if (status != UMI_STATUS_OK) return status;
    status = copy_store_text(coordinator, index, &current, &current_length);
    if (status != UMI_STATUS_OK) return status;
    status = umi_document_store_replace_text(coordinator->store, entry->document_id,
        target, strlen(target));
    if (status == UMI_STATUS_OK) {
        target = redo_direction ? history_pop(entry->redo, &entry->redo_count)
            : history_pop(entry->undo, &entry->undo_count);
        if (redo_direction) HistoryPushOwned(entry->undo, &entry->undo_count, current);
        else HistoryPushOwned(entry->redo, &entry->redo_count, current);
        current = NULL;
        view.dirty = 1;
        if (view.cursor_offset > targetLength) view.cursor_offset = targetLength;
        if (view.selection_length > targetLength - view.cursor_offset)
            view.selection_length = targetLength - view.cursor_offset;
        /* A byte position from the newer text may land inside an older UTF-8
         * character. Keep the restored selection on complete characters. */
        size_t selectionEnd = view.cursor_offset + view.selection_length;
        while (view.cursor_offset > 0U && view.cursor_offset < targetLength &&
            ((unsigned char)target[view.cursor_offset] & 0xc0U) == 0x80U)
            --view.cursor_offset;
        while (selectionEnd > view.cursor_offset && selectionEnd < targetLength &&
            ((unsigned char)target[selectionEnd] & 0xc0U) == 0x80U)
            --selectionEnd;
        view.selection_length = selectionEnd - view.cursor_offset;
        status = UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(coordinator->workbench),
            &view, target, targetLength);
        free(target);
    }
    umi_document_store_free_text(current);
    return status;
}

/*
 * Provide the document coordinator undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_undo(UmiDocumentCoordinator *coordinator)
{
    return coordinator != NULL ? apply_history(coordinator, 0, active_index(coordinator))
                               : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Provide the document coordinator redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_redo(UmiDocumentCoordinator *coordinator)
{
    return coordinator != NULL ? apply_history(coordinator, 1, active_index(coordinator))
                               : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_find(UmiDocumentCoordinator *coordinator,
                                        const char *needle,
                                        size_t *out_offset)
{
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 1U};
    UmiEditorSearchResults results;
    UmiUiDocumentViewSnapshot view;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || needle == NULL || needle[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = active_index(coordinator);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = sync_index(coordinator, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    char *draft = NULL;
    size_t draftLength = 0U;
    status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(coordinator->workbench),
        view.view_id, &draft, &draftLength);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_search_literal(draft, draftLength,
        needle, strlen(needle), &options, &results);
    UmiUiDocumentViewModelFreeText(draft);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK || results.count == 0U) return UMI_STATUS_NOT_FOUND;
    view.cursor_offset = results.matches[0].offset;
    view.selection_length = results.matches[0].byte_count;
    status = umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(coordinator->workbench), &view);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_offset != NULL) *out_offset = results.matches[0].offset;
    return status;
}

/* Prepare the complete new text before changing the store. History entries
 * are preallocated, then transferred only after the store accepts the edit. */
static UmiStatus CommitViewText(UmiDocumentCoordinator *coordinator, size_t index,
    const UmiUiDocumentViewSnapshot *before, UmiUiDocumentViewSnapshot *after,
    const char *beforeText, size_t beforeLength, const char *afterText, size_t afterLength)
{
    char *stored = NULL;
    char *visible = NULL;
    size_t storedLength = 0U;
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    if (before->read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (afterLength > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (beforeLength == afterLength && memcmp(beforeText, afterText, beforeLength) == 0)
        return umi_ui_document_view_model_upsert(views, after);
    UmiStatus status = copy_store_text(coordinator, index, &stored, &storedLength);
    if (status != UMI_STATUS_OK) return status;
    int pendingDraft = storedLength != beforeLength || memcmp(stored, beforeText, storedLength) != 0;
    if (storedLength >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET) {
        umi_document_store_free_text(stored);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (pendingDraft) {
        visible = malloc(beforeLength + 1U);
        if (visible == NULL) { umi_document_store_free_text(stored); return UMI_STATUS_OUT_OF_MEMORY; }
        memcpy(visible, beforeText, beforeLength + 1U);
    }
    /* Reservation changes storage only. Publication below then needs no
     * allocation after the authoritative store has accepted the replacement. */
    status = UmiUiDocumentViewModelReserveText(views, before->view_id, afterLength);
    if (status == UMI_STATUS_OK)
        status = umi_document_store_replace_text(coordinator->store, entry->document_id, afterText, afterLength);
    if (status == UMI_STATUS_OK) {
        HistoryPushOwned(entry->undo, &entry->undo_count, stored);
        stored = NULL;
        if (pendingDraft) {
            HistoryPushOwned(entry->undo, &entry->undo_count, visible);
            visible = NULL;
        }
        history_clear(entry->redo, &entry->redo_count);
        entry->pristine_virtual = 0;
        after->dirty = 1;
        status = UmiUiDocumentViewModelUpsertText(views, after, afterText, afterLength);
    }
    umi_document_store_free_text(stored);
    free(visible);
    return status;
}

/* Replace either the first match (the original API) or the current selection /
 * next match (the interactive API). Invalid input never moves the selection. */
static UmiStatus ReplaceOne(UmiDocumentCoordinator *coordinator, const char *needle,
    const char *replacement, int fromSelection, size_t *outOffset)
{
    UmiUiDocumentViewSnapshot before;
    UmiUiDocumentViewSnapshot after;
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 1U};
    UmiEditorSearchMatch match;
    char *text = NULL;
    char *updated = NULL;
    size_t length = 0U;
    if (coordinator == NULL || needle == NULL || needle[0] == '\0' || replacement == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &before);
    if (status != UMI_STATUS_OK) return status;
    if (before.read_only) return UMI_STATUS_PERMISSION_DENIED;
    status = UmiUiDocumentViewModelCopyText(views, before.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    size_t needleLength = strlen(needle);
    size_t replacementLength = strlen(replacement);
    size_t cursor = before.cursor_offset <= length ? before.cursor_offset : length;
    size_t selection = before.selection_length <= length - cursor ? before.selection_length : length - cursor;
    int useSelection = 0;
    if (fromSelection && selection == needleLength &&
        UmiEditorSearchNavigate(text + cursor, selection, needle, needleLength,
            &options, 0U, 0, 0, &match, NULL) == UMI_STATUS_OK) {
        match.offset = cursor;
        useSelection = 1;
    }
    if (!useSelection)
        status = UmiEditorSearchNavigate(text, length, needle, needleLength, &options,
            fromSelection ? cursor + selection : 0U, 0, fromSelection, &match, NULL);
    if (status == UMI_STATUS_OK) {
        size_t remaining = length - match.byte_count;
        if (replacementLength > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES - remaining)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            size_t updatedLength = remaining + replacementLength;
            updated = malloc(updatedLength + 1U);
            if (updated == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                memcpy(updated, text, match.offset);
                memcpy(updated + match.offset, replacement, replacementLength);
                memcpy(updated + match.offset + replacementLength,
                    text + match.offset + match.byte_count, length - match.offset - match.byte_count + 1U);
                after = before;
                after.cursor_offset = fromSelection ? match.offset + replacementLength : match.offset;
                after.selection_length = fromSelection ? 0U : replacementLength;
                status = CommitViewText(coordinator, index, &before, &after,
                    text, length, updated, updatedLength);
                if (status == UMI_STATUS_OK && outOffset != NULL) *outOffset = match.offset;
            }
        }
    }
    free(updated);
    UmiUiDocumentViewModelFreeText(text);
    return status;
}

/* Move from the current selection, wrapping once at the document boundary. */
UmiStatus UmiDocumentCoordinatorFindNext(UmiDocumentCoordinator *coordinator,
    const char *needle, int backwards, size_t *outOffset, int *outWrapped)
{
    UmiUiDocumentViewSnapshot view;
    UmiEditorSearchMatch match;
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 1U};
    int wrapped = 0;
    if (coordinator == NULL || needle == NULL || needle[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = umi_ui_document_view_model_find(umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    char *text = NULL;
    size_t length = 0U;
    status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(coordinator->workbench),
        view.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    size_t cursor = view.cursor_offset <= length ? view.cursor_offset : length;
    size_t selection = view.selection_length <= length - cursor ? view.selection_length : length - cursor;
    status = UmiEditorSearchNavigate(text, length, needle, strlen(needle), &options,
        backwards ? cursor : cursor + selection, backwards, 1, &match, &wrapped);
    UmiUiDocumentViewModelFreeText(text);
    if (status != UMI_STATUS_OK) return status;
    view.cursor_offset = match.offset;
    view.selection_length = match.byte_count;
    status = umi_ui_document_view_model_upsert(umi_ui_workbench_documents(coordinator->workbench), &view);
    if (status == UMI_STATUS_OK) {
        if (outOffset != NULL) *outOffset = match.offset;
        if (outWrapped != NULL) *outWrapped = wrapped;
    }
    return status;
}

/* The selected match is replaced; otherwise navigation finds the next match. */
UmiStatus UmiDocumentCoordinatorReplaceNext(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, size_t *outOffset)
{
    return ReplaceOne(coordinator, needle, replacement, 1, outOffset);
}

/* Replace All is a single history action. Search, allocation and capacity checks
 * finish before any visible text, saved revision or undo stack is changed. */
UmiStatus UmiDocumentCoordinatorReplaceAll(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, size_t *outCount)
{
    UmiUiDocumentViewSnapshot before;
    UmiUiDocumentViewSnapshot after;
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0};
    size_t count = 0U;
    size_t length = 0U;
    size_t resultLength = 0U;
    char *text = NULL;
    char *updated = NULL;
    if (coordinator == NULL || needle == NULL || needle[0] == '\0' || replacement == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &before);
    if (status != UMI_STATUS_OK) return status;
    if (before.read_only) return UMI_STATUS_PERMISSION_DENIED;
    status = UmiUiDocumentViewModelCopyText(views, before.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    status = UmiEditorSearchReplaceAllSize(text, length, needle, strlen(needle),
        strlen(replacement), &options, &resultLength, &count);
    if (status == UMI_STATUS_OK && resultLength > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && count != 0U) {
        updated = malloc(resultLength + 1U);
        if (updated == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else {
            status = UmiEditorSearchReplaceAll(text, length, needle, strlen(needle),
                replacement, strlen(replacement), &options, updated, resultLength + 1U, &count);
            if (status == UMI_STATUS_OK) {
                after = before;
                if (after.cursor_offset > resultLength) after.cursor_offset = resultLength;
                after.selection_length = 0U;
                status = CommitViewText(coordinator, index, &before, &after,
                    text, length, updated, resultLength);
            }
        }
    }
    free(updated);
    UmiUiDocumentViewModelFreeText(text);
    if (status == UMI_STATUS_OK && outCount != NULL) *outCount = count;
    return status;
}

/*
 * Provide the document coordinator replace operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_replace(UmiDocumentCoordinator *coordinator,
                                           const char *needle,
                                           const char *replacement,
                                           size_t *out_offset)
{
    return ReplaceOne(coordinator, needle, replacement, 0, out_offset);
}

/*
 * Provide the document coordinator go to line operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_go_to_line(
    UmiDocumentCoordinator *coordinator,
    size_t one_based_line,
    size_t *out_offset)
{
    UmiUiDocumentViewSnapshot view;
    size_t index;
    size_t line = 1U;
    size_t offset = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || one_based_line == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    char *text = NULL;
    size_t length = 0U;
    status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(coordinator->workbench),
        view.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    while (offset < length && line < one_based_line) {
        if (text[offset++] == '\n') line += 1U;
    }
    UmiUiDocumentViewModelFreeText(text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (line != one_based_line) return UMI_STATUS_NOT_FOUND;
    view.cursor_offset = offset;
    view.selection_length = 0U;
    status = umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(coordinator->workbench), &view);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_offset != NULL) *out_offset = offset;
    return status;
}

/* Positioning is shared by compiler navigation and other source viewers. */
UmiStatus UmiDocumentCoordinatorGoToPosition(UmiDocumentCoordinator *coordinator,
    size_t oneBasedLine, size_t oneBasedByteColumn, size_t *outOffset)
{
    UmiUiDocumentViewSnapshot view;
    char *text = NULL;
    size_t length = 0U, line = 1U, offset = 0U, end;
    size_t index;
    UmiStatus status;
    if (coordinator == NULL || oneBasedLine == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_ui_document_view_model_find(umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(coordinator->workbench),
        view.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    while (offset < length && line < oneBasedLine) if (text[offset++] == '\n') ++line;
    if (line != oneBasedLine) {
        UmiUiDocumentViewModelFreeText(text); return UMI_STATUS_NOT_FOUND;
    }
    end = offset;
    while (end < length && text[end] != '\n' && text[end] != '\r') ++end;
    if (oneBasedByteColumn != 0U) {
        size_t delta = oneBasedByteColumn - 1U;
        size_t start = offset;
        offset += delta < end - offset ? delta : end - offset;
        while (offset > start && offset < length &&
            ((unsigned char)text[offset] & 0xc0U) == 0x80U) --offset;
    }
    UmiUiDocumentViewModelFreeText(text);
    view.cursor_offset = offset; view.selection_length = 0U;
    status = umi_ui_document_view_model_upsert(umi_ui_workbench_documents(coordinator->workbench), &view);
    if (status == UMI_STATUS_OK && outOffset != NULL) *outOffset = offset;
    return status;
}

/*
 * Provide the document coordinator check external change operation used by this module and
 * its client applications.
 */
UmiStatus umi_document_coordinator_check_external_change(
    UmiDocumentCoordinator *coordinator,
    int *out_changed)
{
    if (out_changed != NULL) *out_changed = 0;
    if (coordinator == NULL || out_changed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return CheckExternalIndex(coordinator, index, out_changed);
}

UmiStatus UmiDocumentCoordinatorCheckExternalChange(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, int *outChanged)
{
    if (outChanged != NULL) *outChanged = 0;
    if (coordinator == NULL || documentId == 0U || outChanged == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < coordinator->count; ++index)
        if (coordinator->entries[index].document_id == documentId)
            return CheckExternalIndex(coordinator, index, outChanged);
    return UMI_STATUS_NOT_FOUND;
}

/* Provide the snapshot index operation used by this module and its client applications. */
static UmiStatus snapshot_index(const UmiDocumentCoordinator *coordinator,
                                size_t index,
                                UmiDocumentWorkingCopySnapshot *out_snapshot)
{
    UmiDocumentSnapshot store_snapshot;
    UmiUiDocumentViewSnapshot view;
    const UmiDocumentCoordinatorEntry *entry;
    UmiStatus status;
    int pendingDraft = 0;
    size_t visibleLength = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= coordinator->count) return UMI_STATUS_NOT_FOUND;
    entry = &coordinator->entries[index];
    status = umi_document_store_snapshot(coordinator->store,
                                         entry->document_id,
                                         &store_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->document_id = entry->document_id;
    (void)snprintf(out_snapshot->view_id, sizeof(out_snapshot->view_id), "%s", entry->view_id);
    (void)snprintf(out_snapshot->path, sizeof(out_snapshot->path), "%s", store_snapshot.path);
    (void)snprintf(out_snapshot->display_name, sizeof(out_snapshot->display_name),
                   "%s", store_snapshot.display_name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (store_snapshot.has_path) {
        (void)umi_document_uri_from_path(store_snapshot.path, out_snapshot->uri,
                                         sizeof(out_snapshot->uri));
    }
    /* Apply this branch only when its contract condition is satisfied. */
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench), entry->view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    {
        char *stored = NULL;
        size_t storedLength = 0U;
        char *visible = NULL;
        status = UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(coordinator->workbench),
            view.view_id, &visible, &visibleLength);
        if (status != UMI_STATUS_OK) return status;
        status = umi_document_store_copy_text(coordinator->store, entry->document_id, &stored, &storedLength);
        if (status == UMI_STATUS_OK)
            pendingDraft = visibleLength != storedLength || memcmp(stored, visible, storedLength) != 0;
        umi_document_store_free_text(stored);
        UmiUiDocumentViewModelFreeText(visible);
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(out_snapshot->language_id,
                       sizeof(out_snapshot->language_id), "%.*s",
                       (int)sizeof(out_snapshot->language_id) - 1,
                       view.language_id);
        out_snapshot->active = view.active;
    }
    out_snapshot->encoding = entry->encoding;
    out_snapshot->line_ending = entry->line_ending;
    out_snapshot->conflict = entry->conflict;
    out_snapshot->text_length = visibleLength;
    out_snapshot->undo_count = entry->undo_count;
    out_snapshot->redo_count = entry->redo_count;
    out_snapshot->revision = store_snapshot.revision;
    out_snapshot->dirty = pendingDraft || view.dirty || (!entry->pristine_virtual &&
        (store_snapshot.dirty || !store_snapshot.has_path));
    out_snapshot->has_path = store_snapshot.has_path;
    return UMI_STATUS_OK;
}

/*
 * Provide the document coordinator active snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_active_snapshot(
    const UmiDocumentCoordinator *coordinator,
    UmiDocumentWorkingCopySnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    return index == SIZE_MAX ? UMI_STATUS_NOT_FOUND
                             : snapshot_index(coordinator, index, out_snapshot);
}

/*
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_at(
    const UmiDocumentCoordinator *coordinator,
    size_t index,
    UmiDocumentWorkingCopySnapshot *out_snapshot)
{
    return snapshot_index(coordinator, index, out_snapshot);
}

/*
 * Return the number of records represented by document coordinator without changing their
 * state.
 */
size_t umi_document_coordinator_count(
    const UmiDocumentCoordinator *coordinator)
{
    return coordinator != NULL ? coordinator->count : 0U;
}

UmiStatus UmiDocumentCoordinatorOpenSearchMatch(UmiDocumentCoordinator *coordinator,
    const UmiSearchMatch *match, const char *query, int caseSensitive,
    size_t *outOffset)
{
    char viewId[UMI_UI_ID_CAPACITY];
    char *text = NULL;
    size_t length = 0U, offset = 0U;
    UmiStatus status;
    if (coordinator == NULL || match == NULL || query == NULL || query[0] == '\0' ||
        match->line == 0U || match->column == 0U ||
        memchr(match->path, '\0', sizeof match->path) == NULL ||
        !umi_path_is_absolute(match->path)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_document_coordinator_open(coordinator, match->path, viewId, sizeof viewId);
    if (status != UMI_STATUS_OK) return status;
    status = UmiUiDocumentViewModelCopyText(
        umi_ui_workbench_documents(coordinator->workbench), viewId, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    status = UmiSearchMatchLocate(match, query, caseSensitive, text, length, &offset);
    UmiUiDocumentViewModelFreeText(text);
    if (status != UMI_STATUS_OK) return status;
    return UmiDocumentCoordinatorGoToPosition(coordinator, match->line,
        match->column, outOffset);
}

/* A reload is reviewed against two independently changing sources: the visible
 * draft and the provider resource. Neither is changed during preparation. */
struct UmiDocumentReloadPlan {
    const UmiDocumentCoordinator *owner;
    UmiDocumentSnapshot expected;
    UmiUiDocumentTextInfo expectedText;
    UmiDocumentFingerprint expectedBaseline;
    char viewId[UMI_UI_ID_CAPACITY];
    char viewDocumentId[UMI_UI_ID_CAPACITY];
    char *previousText;
    UmiDocumentLoadResult incoming;
    UmiDocumentReloadSummary summary;
    int expectedDirtyMarker;
    int consumed;
};

void UmiDocumentReloadPlanDestroy(UmiDocumentReloadPlan *plan)
{
    if (plan == NULL) return;
    UmiUiDocumentViewModelFreeText(plan->previousText);
    umi_document_load_result_dispose(&plan->incoming);
    free(plan);
}

UmiStatus UmiDocumentReloadPlanSummary(const UmiDocumentReloadPlan *plan,
    UmiDocumentReloadSummary *outSummary)
{
    if (plan == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    *outSummary = plan->summary;
    return UMI_STATUS_OK;
}

/* A review shows the captured bytes, not a fresh read which could disagree
 * with the plan the user later approves. The plan keeps ownership throughout. */
UmiStatus UmiDocumentReloadPlanTexts(const UmiDocumentReloadPlan *plan,
    const char **outPrevious, size_t *outPreviousLength,
    const char **outIncoming, size_t *outIncomingLength)
{
    if (outPrevious != NULL) *outPrevious = NULL;
    if (outPreviousLength != NULL) *outPreviousLength = 0U;
    if (outIncoming != NULL) *outIncoming = NULL;
    if (outIncomingLength != NULL) *outIncomingLength = 0U;
    if (plan == NULL || outPrevious == NULL || outPreviousLength == NULL ||
        outIncoming == NULL || outIncomingLength == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    *outPrevious = plan->previousText;
    *outPreviousLength = plan->summary.previous_bytes;
    *outIncoming = plan->incoming.text;
    *outIncomingLength = plan->incoming.text_length;
    return UMI_STATUS_OK;
}

/* All coordinator and view mutations occur on their common owner thread. The
 * store still compares its revision under its own mutex when publishing. */
static UmiStatus ReloadCurrent(UmiDocumentCoordinator *coordinator,
    const UmiDocumentReloadPlan *plan, size_t *outIndex,
    UmiUiDocumentViewSnapshot *outView)
{
    UmiDocumentSnapshot stored;
    UmiUiDocumentTextInfo text;
    if (coordinator == NULL || plan == NULL || plan->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    size_t index = find_view(coordinator, plan->viewId);
    if (index == SIZE_MAX || coordinator->entries[index].document_id != plan->expected.document_id)
        return UMI_STATUS_NOT_FOUND;
    UmiStatus status = umi_document_store_snapshot(coordinator->store,
        plan->expected.document_id, &stored);
    if (status != UMI_STATUS_OK) return status;
    if (stored.revision != plan->expected.revision ||
        stored.saved_revision != plan->expected.saved_revision ||
        !stored.has_path || strcmp(stored.path, plan->expected.path) != 0)
        return UMI_STATUS_INVALID_STATE;
    const UmiDocumentFingerprint *baseline = &coordinator->entries[index].baseline;
    if (baseline->valid != plan->expectedBaseline.valid ||
        (baseline->valid && !umi_document_fingerprint_equal(baseline, &plan->expectedBaseline)))
        return UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    status = umi_ui_document_view_model_find(views, plan->viewId, outView);
    if (status != UMI_STATUS_OK) return status;
    status = UmiUiDocumentViewModelTextInfo(views, plan->viewId, &text);
    if (status != UMI_STATUS_OK) return status;
    if (text.text_revision != plan->expectedText.text_revision ||
        strcmp(outView->document_id, plan->viewDocumentId) != 0 ||
        outView->dirty != plan->expectedDirtyMarker)
        return UMI_STATUS_INVALID_STATE;
    *outIndex = index;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorPrepareReload(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentReloadPlan **outPlan)
{
    size_t index = SIZE_MAX;
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentLoadOptions options = umi_document_load_options_default();
    if (outPlan != NULL) *outPlan = NULL;
    if (coordinator == NULL || documentId == 0U || outPlan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t candidate = 0U; candidate < coordinator->count; ++candidate)
        if (coordinator->entries[candidate].document_id == documentId) { index = candidate; break; }
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = snapshot_index(coordinator, index, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.has_path) return UMI_STATUS_INVALID_STATE;
    UmiDocumentReloadPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->owner = coordinator;
    plan->expectedBaseline = coordinator->entries[index].baseline;
    (void)snprintf(plan->viewId, sizeof plan->viewId, "%s", snapshot.view_id);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    status = umi_document_store_snapshot(coordinator->store, documentId, &plan->expected);
    if (status == UMI_STATUS_OK)
        status = umi_ui_document_view_model_find(views, plan->viewId, &view);
    if (status == UMI_STATUS_OK) {
        plan->expectedDirtyMarker = view.dirty;
        (void)snprintf(plan->viewDocumentId, sizeof plan->viewDocumentId, "%s", view.document_id);
        status = UmiUiDocumentViewModelTextInfo(views, plan->viewId, &plan->expectedText);
    }
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentViewModelCopyText(views, plan->viewId,
            &plan->previousText, &plan->summary.previous_bytes);
    options.maximum_bytes = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
    options.normalise_to = UMI_DOCUMENT_LINE_ENDING_LF;
    if (status == UMI_STATUS_OK)
        status = umi_document_load(&coordinator->provider, plan->expected.path, &options, &plan->incoming);
    if (status == UMI_STATUS_OK && plan->incoming.text_length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    /* A provider may dispatch a callback. Verify the target again after I/O. */
    if (status == UMI_STATUS_OK) status = ReloadCurrent(coordinator, plan, &index, &view);
    if (status != UMI_STATUS_OK) {
        UmiDocumentReloadPlanDestroy(plan);
        return status;
    }
    plan->summary.document_id = documentId;
    (void)snprintf(plan->summary.display_name, sizeof plan->summary.display_name, "%s", snapshot.display_name);
    plan->summary.incoming_bytes = plan->incoming.text_length;
    plan->summary.has_unsaved_changes = snapshot.dirty;
    plan->summary.incoming_encoding = plan->incoming.detected_encoding;
    plan->summary.incoming_line_ending = plan->incoming.detected_line_ending;
    plan->summary.text_changes = plan->summary.previous_bytes != plan->incoming.text_length ||
        memcmp(plan->previousText, plan->incoming.text, plan->summary.previous_bytes) != 0;
    *outPlan = plan;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorApplyReload(UmiDocumentCoordinator *coordinator,
    UmiDocumentReloadPlan *plan, int discardUnsaved)
{
    UmiUiDocumentViewSnapshot view;
    UmiDocumentFingerprint current;
    size_t index = 0U;
    if (discardUnsaved != 0 && discardUnsaved != 1) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ReloadCurrent(coordinator, plan, &index, &view);
    if (status != UMI_STATUS_OK) return status;
    if (plan->summary.has_unsaved_changes && !discardUnsaved) return UMI_STATUS_INVALID_STATE;
    if (view.read_only && plan->summary.has_unsaved_changes) return UMI_STATUS_PERMISSION_DENIED;
    status = UmiDocumentFingerprintRead(&coordinator->provider, plan->expected.path,
        UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES, &current);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_document_fingerprint_equal(&current, &plan->incoming.fingerprint))
        return UMI_STATUS_INVALID_STATE;
    status = ReloadCurrent(coordinator, plan, &index, &view);
    if (status != UMI_STATUS_OK) return status;
    if (view.read_only && plan->summary.has_unsaved_changes) return UMI_STATUS_PERMISSION_DENIED;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    size_t length = plan->incoming.text_length;
    /* No allocation is needed in view publication after the store commits. */
    status = UmiUiDocumentViewModelReserveText(views, plan->viewId, length);
    if (status != UMI_STATUS_OK) return status;
    status = UmiDocumentStoreReplaceLoaded(coordinator->store, plan->expected.document_id,
        plan->expected.revision, plan->expected.saved_revision, plan->expected.path,
        plan->incoming.text, length);
    if (status != UMI_STATUS_OK) return status;
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    if (plan->summary.text_changes) {
        HistoryPushOwned(entry->undo, &entry->undo_count, plan->previousText);
        plan->previousText = NULL;
        history_clear(entry->redo, &entry->redo_count);
    }
    entry->baseline = plan->incoming.fingerprint;
    entry->encoding = plan->incoming.detected_encoding;
    entry->line_ending = plan->incoming.detected_line_ending;
    if (entry->line_ending == UMI_DOCUMENT_LINE_ENDING_NONE ||
        entry->line_ending == UMI_DOCUMENT_LINE_ENDING_MIXED)
        entry->line_ending = UMI_DOCUMENT_LINE_ENDING_LF;
    entry->conflict = UMI_DOCUMENT_CONFLICT_NONE;
    entry->pristine_virtual = 0;
    view.dirty = 0;
    if (view.cursor_offset > length) view.cursor_offset = length;
    while (view.cursor_offset > 0U && view.cursor_offset < length &&
        ((unsigned char)plan->incoming.text[view.cursor_offset] & 0xc0U) == 0x80U)
        --view.cursor_offset;
    if (plan->summary.text_changes) view.selection_length = 0U;
    if (view.selection_length > length - view.cursor_offset)
        view.selection_length = length - view.cursor_offset;
    status = UmiUiDocumentViewModelUpsertText(views, &view, plan->incoming.text, length);
    plan->consumed = 1;
    return status;
}


/* Document editing shares CommitViewText and the original history stacks.
 * A plan is an immutable capture, not another document model. */
struct UmiDocumentEditPlan {
    const UmiDocumentCoordinator *owner;
    UmiDocumentSnapshot stored;
    UmiUiDocumentTextInfo textInfo;
    UmiUiDocumentViewSnapshot view;
    char *text;
    size_t length;
    int consumed;
};

static size_t EditDocumentIndex(const UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId)
{
    for (size_t index = 0U; index < coordinator->count; ++index)
        if (coordinator->entries[index].document_id == documentId) return index;
    return SIZE_MAX;
}

UmiStatus UmiDocumentEditCommandFromId(const char *commandId,
    UmiDocumentEditCommand *outCommand)
{
    static const char *const names[] = {"edit.undo", "edit.redo", "edit.cut",
        "edit.copy", "edit.paste", "edit.select-all", "edit.delete"};
    if (commandId == NULL || outCommand == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < sizeof names / sizeof names[0]; ++index) {
        if (strcmp(commandId, names[index]) == 0) {
            *outCommand = (UmiDocumentEditCommand)(index + 1U);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

void UmiDocumentEditPlanDestroy(UmiDocumentEditPlan *plan)
{
    if (plan == NULL) return;
    UmiUiDocumentViewModelFreeText(plan->text);
    free(plan);
}

/* Selection offsets are byte positions. Neither endpoint may split a UTF-8
 * character. A view with invalid metadata must fail before memcpy or editing. */
static int EditSelectionValid(const UmiUiDocumentViewSnapshot *view,
    const char *text, size_t length)
{
    if (view->cursor_offset > length ||
        view->selection_length > length - view->cursor_offset) return 0;
    size_t end = view->cursor_offset + view->selection_length;
    return (view->cursor_offset == length ||
            ((unsigned char)text[view->cursor_offset] & 0xc0U) != 0x80U) &&
        (end == length || ((unsigned char)text[end] & 0xc0U) != 0x80U);
}

UmiStatus UmiDocumentCoordinatorPrepareEdit(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentEditPlan **outPlan)
{
    if (outPlan != NULL) *outPlan = NULL;
    if (coordinator == NULL || documentId == 0U || outPlan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiDocumentEditPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->owner = coordinator;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_document_store_snapshot(coordinator->store, documentId, &plan->stored);
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_find(views,
        coordinator->entries[index].view_id, &plan->view);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelTextInfo(views,
        plan->view.view_id, &plan->textInfo);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views,
        plan->view.view_id, &plan->text, &plan->length);
    if (status == UMI_STATUS_OK && !EditSelectionValid(&plan->view, plan->text, plan->length))
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) { UmiDocumentEditPlanDestroy(plan); return status; }
    *outPlan = plan;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentEditPlanSelection(const UmiDocumentEditPlan *plan,
    const char **outText, size_t *outBytes)
{
    if (outText != NULL) *outText = NULL;
    if (outBytes != NULL) *outBytes = 0U;
    if (plan == NULL || outText == NULL || outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    *outText = plan->text + plan->view.cursor_offset;
    *outBytes = plan->view.selection_length;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorGetEditState(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentEditState *outState)
{
    if (coordinator == NULL || documentId == 0U || outState == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentTextInfo info;
    UmiDocumentSnapshot storedInfo;
    UmiStatus status = umi_ui_document_view_model_find(views, entry->view_id, &view);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelTextInfo(views, entry->view_id, &info);
    if (status == UMI_STATUS_OK) status = umi_document_store_snapshot(coordinator->store, documentId, &storedInfo);
    if (status != UMI_STATUS_OK) return status;
    if (view.cursor_offset > info.byte_count || view.selection_length > info.byte_count - view.cursor_offset)
        return UMI_STATUS_INVALID_STATE;
    if (!entry->edit_cache_valid || entry->edit_text_revision != info.text_revision ||
        entry->edit_store_revision != storedInfo.revision) {
        char *stored = NULL, *draft = NULL;
        size_t storedLength = 0U, draftLength = 0U;
        status = copy_store_text(coordinator, index, &stored, &storedLength);
        if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views, entry->view_id, &draft, &draftLength);
        if (status == UMI_STATUS_OK) {
            entry->edit_pending = storedLength != draftLength || memcmp(stored, draft, storedLength) != 0;
            entry->edit_text_revision = info.text_revision;
            entry->edit_store_revision = storedInfo.revision;
            entry->edit_cache_valid = 1;
        }
        umi_document_store_free_text(stored);
        UmiUiDocumentViewModelFreeText(draft);
        if (status != UMI_STATUS_OK) return status;
    }
    UmiDocumentEditState state = {0};
    state.document_id = documentId;
    state.text_bytes = info.byte_count;
    state.selection_offset = view.cursor_offset;
    state.selection_bytes = view.selection_length;
    state.read_only = view.read_only;
    state.can_undo = !state.read_only && (entry->edit_pending || entry->undo_count != 0U);
    state.can_redo = !state.read_only && !entry->edit_pending && entry->redo_count != 0U;
    *outState = state;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorUndo(UmiDocumentCoordinator *coordinator, UmiDocumentId documentId)
{
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return apply_history(coordinator, 0, EditDocumentIndex(coordinator, documentId));
}

UmiStatus UmiDocumentCoordinatorRedo(UmiDocumentCoordinator *coordinator, UmiDocumentId documentId)
{
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return apply_history(coordinator, 1, EditDocumentIndex(coordinator, documentId));
}

UmiStatus UmiDocumentCoordinatorSelectAll(UmiDocumentCoordinator *coordinator, UmiDocumentId documentId)
{
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentTextInfo text;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &view);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelTextInfo(views, view.view_id, &text);
    if (status == UMI_STATUS_OK) {
        view.cursor_offset = 0U;
        view.selection_length = text.byte_count;
        status = umi_ui_document_view_model_upsert(views, &view);
    }
    return status;
}

UmiStatus UmiDocumentCoordinatorApplyEdit(UmiDocumentCoordinator *coordinator,
    UmiDocumentEditPlan *plan, const char *replacement, size_t bytes)
{
    if (coordinator == NULL || plan == NULL || replacement == NULL || plan->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    if (bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(replacement, '\0', bytes) != NULL ||
        !umi_document_utf8_validate((const unsigned char *)replacement, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, plan->stored.document_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot current;
    UmiUiDocumentTextInfo textInfo;
    UmiDocumentSnapshot stored;
    UmiStatus status = umi_document_store_snapshot(coordinator->store, plan->stored.document_id, &stored);
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_find(views,
        coordinator->entries[index].view_id, &current);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelTextInfo(views, current.view_id, &textInfo);
    if (status != UMI_STATUS_OK) return status;
    if (current.read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (stored.revision != plan->stored.revision || stored.saved_revision != plan->stored.saved_revision ||
        stored.has_path != plan->stored.has_path || strcmp(stored.path, plan->stored.path) != 0 ||
        textInfo.text_revision != plan->textInfo.text_revision ||
        strcmp(current.view_id, plan->view.view_id) != 0 ||
        strcmp(current.document_id, plan->view.document_id) != 0 ||
        strcmp(current.uri, plan->view.uri) != 0 ||
        current.cursor_offset != plan->view.cursor_offset ||
        current.selection_length != plan->view.selection_length)
        return UMI_STATUS_INVALID_STATE;
    size_t remaining = plan->length - plan->view.selection_length;
    if (bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES - remaining) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t newLength = remaining + bytes;
    char *newText = malloc(newLength + 1U);
    if (newText == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t offset = plan->view.cursor_offset;
    memcpy(newText, plan->text, offset);
    memcpy(newText + offset, replacement, bytes);
    size_t tail = offset + plan->view.selection_length;
    memcpy(newText + offset + bytes, plan->text + tail, plan->length - tail);
    newText[newLength] = '\0';
    UmiUiDocumentViewSnapshot after = current;
    after.cursor_offset = offset + bytes;
    after.selection_length = 0U;
    after.preview = 0;
    status = CommitViewText(coordinator, index, &current, &after,
        plan->text, plan->length, newText, newLength);
    free(newText);
    if (status == UMI_STATUS_OK) plan->consumed = 1;
    return status;
}

UmiStatus UmiDocumentCoordinatorSyncDocument(UmiDocumentCoordinator *coordinator, UmiDocumentId documentId)
{
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    return index == SIZE_MAX ? UMI_STATUS_NOT_FOUND : sync_index(coordinator, index);
}
