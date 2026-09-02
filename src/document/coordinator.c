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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/document/file_identity.h"
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
} UmiDocumentCoordinatorEntry;

struct UmiDocumentCoordinator {
    UmiDocumentStore *store;
    UmiUiWorkbench *workbench;
    UmiDocumentProvider provider;
    UmiDocumentCoordinatorEntry entries[UMI_DOCUMENT_MAX_WORKING_COPIES];
    size_t count;
    uint64_t next_untitled;
};

/* Release or reset state held by history so the same storage can be reused safely. */
static void history_clear(char **items, size_t *count)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < *count; ++index) free(items[index]);
    *count = 0U;
}

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
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(copy, text, length + 1U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count == UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY) {
        free(items[0]);
        (void)memmove(&items[0], &items[1],
                      (UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY - 1U) * sizeof(items[0]));
        *count -= 1U;
    }
    items[(*count)++] = copy;
    return UMI_STATUS_OK;
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
    status = umi_document_store_snapshot(coordinator->store,
                                         entry->document_id,
                                         &store_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_store_text(coordinator, index, &text, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_UI_DOCUMENT_CONTENT_CAPACITY) {
        umi_document_store_free_text(text);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&view, 0, sizeof(view));
    (void)snprintf(view.view_id, sizeof(view.view_id), "%s", entry->view_id);
    (void)snprintf(view.document_id, sizeof(view.document_id),
                   "document.%llu", (unsigned long long)entry->document_id);
    (void)snprintf(view.title, sizeof(view.title), "%s", store_snapshot.display_name);
    (void)snprintf(view.source_text, sizeof(view.source_text), "%s", text);
    view.dirty = store_snapshot.dirty || !store_snapshot.has_path;
    view.active = 1;
    view.pinned = 1;
    view.cursor_offset = 0U;
    view.selection_length = 0U;
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
    umi_document_store_free_text(text);
    status = umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(coordinator->workbench), &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
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
        status = umi_document_store_replace_text(coordinator->store,
                                                 document_id,
                                                 view.source_text,
                                                 strlen(view.source_text));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = register_entry(coordinator, document_id,
                                UMI_DOCUMENT_ENCODING_UTF8,
                                UMI_DOCUMENT_LINE_ENDING_LF,
                                NULL, view.view_id, &entry_index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
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
    UmiDocumentId document_id;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = refresh_view(coordinator, index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_view_id != NULL && capacity > 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(coordinator->entries[index].view_id) + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(out_view_id, capacity, "%s", coordinator->entries[index].view_id);
    }
    return status;
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
    UmiDocumentId document_id;
    const char *name;
    const char *separator;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    options.maximum_bytes = UMI_UI_DOCUMENT_CONTENT_CAPACITY - 1U;
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = refresh_view(coordinator, index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_view_id != NULL && capacity > 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(coordinator->entries[index].view_id) + 1U > capacity) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)snprintf(out_view_id, capacity, "%s", coordinator->entries[index].view_id);
        }
    }
    umi_document_load_result_dispose(&loaded);
    return status;
}

/* Provide the sync index operation used by this module and its client applications. */
static UmiStatus sync_index(UmiDocumentCoordinator *coordinator, size_t index)
{
    UmiUiDocumentViewSnapshot view;
    char *stored = NULL;
    size_t length = 0U;
    UmiStatus status;
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench),
        coordinator->entries[index].view_id, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_store_text(coordinator, index, &stored, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(stored, view.source_text) != 0) {
        status = history_push(coordinator->entries[index].undo,
                              &coordinator->entries[index].undo_count,
                              stored);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            history_clear(coordinator->entries[index].redo,
                          &coordinator->entries[index].redo_count);
            status = umi_document_store_replace_text(
                coordinator->store, coordinator->entries[index].document_id,
                view.source_text, strlen(view.source_text));
        }
    }
    umi_document_store_free_text(stored);
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
    UmiStatus status = sync_index(coordinator, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this operation only while the related capability or state is available. */
    if (entry->baseline.valid) {
        UmiDocumentSnapshot snapshot;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_document_store_snapshot(coordinator->store, entry->document_id,
                                        &snapshot) == UMI_STATUS_OK &&
            snapshot.has_path && strcmp(snapshot.path, path) == 0) {
            UmiStatus check = umi_document_file_changed(path, &entry->baseline,
                                                        &changed, NULL);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if ((check == UMI_STATUS_OK && changed) || check == UMI_STATUS_NOT_FOUND) {
                entry->conflict = changed ? UMI_DOCUMENT_CONFLICT_EXTERNAL_CHANGE
                                          : UMI_DOCUMENT_CONFLICT_DELETED_EXTERNALLY;
                (void)umi_document_store_mark_external_change(
                    coordinator->store, entry->document_id, 1);
                return UMI_STATUS_INVALID_STATE;
            }
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

/*
 * Provide the document coordinator close active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_close_active(
    UmiDocumentCoordinator *coordinator,
    int force)
{
    size_t index;
    size_t move_count;
    UmiDocumentSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
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
    if (!force && (snapshot.dirty || !snapshot.has_path)) return UMI_STATUS_INVALID_STATE;
    status = umi_document_store_close(coordinator->store,
                                      coordinator->entries[index].document_id,
                                      force);
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
    if (coordinator->count > 0U) {
        size_t activate = index < coordinator->count ? index : coordinator->count - 1U;
        return umi_ui_workbench_activate_document(coordinator->workbench,
                                                  coordinator->entries[activate].view_id);
    }
    return UMI_STATUS_OK;
}

/* Provide the apply history operation used by this module and its client applications. */
static UmiStatus apply_history(UmiDocumentCoordinator *coordinator,
                               int redo_direction)
{
    size_t index = active_index(coordinator);
    UmiDocumentCoordinatorEntry *entry;
    char *current = NULL;
    size_t current_length = 0U;
    char *target;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = sync_index(coordinator, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry = &coordinator->entries[index];
    target = redo_direction ? history_pop(entry->redo, &entry->redo_count)
                            : history_pop(entry->undo, &entry->undo_count);
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_store_text(coordinator, index, &current, &current_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = redo_direction
            ? history_push(entry->undo, &entry->undo_count, current)
            : history_push(entry->redo, &entry->redo_count, current);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_document_store_replace_text(coordinator->store,
                                                 entry->document_id,
                                                 target, strlen(target));
    }
    free(target);
    umi_document_store_free_text(current);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = refresh_view(coordinator, index);
    return status;
}

/*
 * Provide the document coordinator undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_undo(UmiDocumentCoordinator *coordinator)
{
    return coordinator != NULL ? apply_history(coordinator, 0)
                               : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Provide the document coordinator redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_redo(UmiDocumentCoordinator *coordinator)
{
    return coordinator != NULL ? apply_history(coordinator, 1)
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
    status = umi_editor_search_literal(view.source_text, strlen(view.source_text),
                                       needle, strlen(needle), &options, &results);
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

/*
 * Provide the document coordinator replace operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_replace(UmiDocumentCoordinator *coordinator,
                                           const char *needle,
                                           const char *replacement,
                                           size_t *out_offset)
{
    size_t offset;
    size_t index;
    char *current = NULL;
    size_t length = 0U;
    size_t needle_length;
    size_t replacement_length;
    char *updated;
    UmiStatus status = umi_document_coordinator_find(coordinator, needle, &offset);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || replacement == NULL) return status;
    index = active_index(coordinator);
    status = copy_store_text(coordinator, index, &current, &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    needle_length = strlen(needle);
    replacement_length = strlen(replacement);
    updated = (char *)malloc(length - needle_length + replacement_length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (updated == NULL) {
        umi_document_store_free_text(current);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(updated, current, offset);
    (void)memcpy(updated + offset, replacement, replacement_length);
    (void)memcpy(updated + offset + replacement_length,
                 current + offset + needle_length,
                 length - offset - needle_length + 1U);
    status = history_push(coordinator->entries[index].undo,
                          &coordinator->entries[index].undo_count, current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        history_clear(coordinator->entries[index].redo,
                      &coordinator->entries[index].redo_count);
        status = umi_document_store_replace_text(coordinator->store,
                                                 coordinator->entries[index].document_id,
                                                 updated,
                                                 length - needle_length + replacement_length);
    }
    free(updated);
    umi_document_store_free_text(current);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = refresh_view(coordinator, index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_offset != NULL) *out_offset = offset;
    return status;
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
    while (view.source_text[offset] != '\0' && line < one_based_line) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (view.source_text[offset++] == '\n') line += 1U;
    }
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

/*
 * Provide the document coordinator check external change operation used by this module and
 * its client applications.
 */
UmiStatus umi_document_coordinator_check_external_change(
    UmiDocumentCoordinator *coordinator,
    int *out_changed)
{
    size_t index;
    UmiDocumentSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_changed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = active_index(coordinator);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_document_store_snapshot(coordinator->store,
                                         coordinator->entries[index].document_id,
                                         &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!snapshot.has_path || !coordinator->entries[index].baseline.valid) {
        *out_changed = 0;
        return UMI_STATUS_OK;
    }
    status = umi_document_file_changed(snapshot.path,
                                       &coordinator->entries[index].baseline,
                                       out_changed, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && *out_changed) {
        coordinator->entries[index].conflict = UMI_DOCUMENT_CONFLICT_EXTERNAL_CHANGE;
        (void)umi_document_store_mark_external_change(
            coordinator->store, coordinator->entries[index].document_id, 1);
    }
    return status;
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
    if (umi_ui_document_view_model_find(
            umi_ui_workbench_documents(coordinator->workbench), entry->view_id,
            &view) == UMI_STATUS_OK) {
        (void)snprintf(out_snapshot->language_id,
                       sizeof(out_snapshot->language_id), "%.*s",
                       (int)sizeof(out_snapshot->language_id) - 1,
                       view.language_id);
        out_snapshot->active = view.active;
    }
    out_snapshot->encoding = entry->encoding;
    out_snapshot->line_ending = entry->line_ending;
    out_snapshot->conflict = entry->conflict;
    out_snapshot->text_length = store_snapshot.length;
    out_snapshot->undo_count = entry->undo_count;
    out_snapshot->redo_count = entry->redo_count;
    out_snapshot->revision = store_snapshot.revision;
    out_snapshot->dirty = store_snapshot.dirty || !store_snapshot.has_path;
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
