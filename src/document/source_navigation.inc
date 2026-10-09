/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_navigation.inc
 * PURPOSE: Resolve protocol positions before selecting a live source document without saving or editing it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_navigation.h"
#include "umicom/document/local_uri.h"
#include "umicom/editor/text_position_index.h"
UmiStatus UmiDocumentCoordinatorFindSourceUri(const UmiDocumentCoordinator *coordinator, const char *uri,
                                              UmiDocumentId *out_document)
{
    if (coordinator == NULL || uri == NULL || uri[0] == '\0' || out_document == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiDocumentId found = 0U;
    for (size_t i = 0U; i < coordinator->count; ++i)
    {
        UmiUiDocumentViewSnapshot view;
        UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[i].view_id, &view);
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(view.uri, uri) == 0)
        {
            /* Ambiguous contributed identities must not silently select the
             * first tab. The host can repair duplicate metadata and retry. */
            if (found != 0U)
                return UMI_STATUS_ALREADY_EXISTS;
            found = coordinator->entries[i].document_id;
        }
    }
    if (found == 0U)
        return UMI_STATUS_NOT_FOUND;
    *out_document = found;
    return UMI_STATUS_OK;
}
static UmiStatus SourceNavigationOffsets(const char *text, size_t bytes, UmiEditorTextPosition start,
                                         UmiEditorTextPosition end, size_t *first, size_t *last)
{
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = view.capacity = bytes;
    UmiEditorTextPositionIndex *index = NULL;
    UmiStatus status = UmiEditorTextPositionIndexCreate(&view, NULL, &index);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(index, start, first);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(index, end, last);
    if (status == UMI_STATUS_OK && *last < *first)
        status = UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextPositionIndexDestroy(index);
    return status;
}
static UmiStatus SourceNavigationSelect(UmiDocumentCoordinator *coordinator, UmiDocumentId document,
                                        UmiEditorTextPosition start, UmiEditorTextPosition end,
                                        size_t *out_offset, size_t *out_bytes)
{
    size_t index = EditDocumentIndex(coordinator, document);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &before);
    char *text = NULL;
    size_t bytes = 0U, first = 0U, last = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentViewModelCopyText(views, before.view_id, &text, &bytes);
    if (status == UMI_STATUS_OK)
        status = SourceNavigationOffsets(text, bytes, start, end, &first, &last);
    UmiUiDocumentViewModelFreeText(text);
    if (status != UMI_STATUS_OK)
        return status;
    UmiUiDocumentViewSnapshot after = before;
    after.cursor_offset = first;
    after.selection_length = last - first;
    /* Publish selection only after all source validation and allocation.
     * If activation fails, restore metadata on the same identified view. */
    status = umi_ui_document_view_model_upsert(views, &after);
    if (status == UMI_STATUS_OK)
    {
        status = umi_ui_workbench_activate_document(coordinator->workbench, after.view_id);
        if (status != UMI_STATUS_OK)
            (void)umi_ui_document_view_model_upsert(views, &before);
    }
    if (status == UMI_STATUS_OK)
    {
        if (out_offset != NULL)
            *out_offset = first;
        if (out_bytes != NULL)
            *out_bytes = last - first;
    }
    return status;
}
UmiStatus UmiDocumentCoordinatorSelectSourceRange(UmiDocumentCoordinator *coordinator, UmiDocumentId document,
                                                  UmiEditorTextPosition start, UmiEditorTextPosition end,
                                                  size_t *out_offset, size_t *out_bytes)
{
    if (coordinator == NULL || document == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    UmiEditorSourceLocation departure;
    UmiStatus captured = NavigationCapture(coordinator, &departure);
    ++coordinator->navigation_depth;
    UmiStatus status = SourceNavigationSelect(coordinator, document, start, end, out_offset, out_bytes);
    --coordinator->navigation_depth;
    if (status == UMI_STATUS_OK)
        NavigationRecord(coordinator, &departure, captured);
    return status;
}

static UmiStatus SourceNavigationFindPath(const UmiDocumentCoordinator *coordinator, const char *path,
                                          UmiDocumentId *out_document)
{
    for (size_t i = 0U; i < coordinator->count; ++i)
    {
        UmiDocumentSnapshot stored;
        UmiStatus status =
            umi_document_store_snapshot(coordinator->store, coordinator->entries[i].document_id, &stored);
        if (status != UMI_STATUS_OK)
            return status;
        if (stored.has_path && umi_path_equal(stored.path, path))
        {
            *out_document = stored.document_id;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
/* Roll back only the new, unedited attachment owned by this navigation. Do not
 * use user Close: a failed navigation should not create a reopen-history item. */
static void SourceNavigationDiscard(UmiDocumentCoordinator *coordinator, UmiDocumentId document)
{
    size_t index = EditDocumentIndex(coordinator, document);
    if (index == SIZE_MAX)
        return;
    (void)umi_ui_document_view_model_remove(umi_ui_workbench_documents(coordinator->workbench),
                                            coordinator->entries[index].view_id);
    entry_dispose(&coordinator->entries[index]);
    if (index + 1U < coordinator->count)
        memmove(&coordinator->entries[index], &coordinator->entries[index + 1U],
                (coordinator->count - index - 1U) * sizeof(coordinator->entries[0]));
    --coordinator->count;
    memset(&coordinator->entries[coordinator->count], 0, sizeof(coordinator->entries[0]));
    (void)umi_document_store_close(coordinator->store, document, 1);
}
/* Source navigation now uses the same encoded-file envelope as ordinary open; attaching a decoded draft still enforces the editor text limit.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentCoordinatorOpenSourceRange(UmiDocumentCoordinator *coordinator, const char *uri,
                                                UmiEditorTextPosition start, UmiEditorTextPosition end,
                                                UmiDocumentId *out_document, size_t *out_offset,
                                                size_t *out_bytes)
{
    if (coordinator == NULL || uri == NULL || uri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t bytes = 0U;
    while (bytes <= 8192U && uri[bytes] != '\0')
        ++bytes;
    if (bytes > 8192U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char captured_uri[8193], path[UMI_PATH_CAPACITY], previous[UMI_UI_ID_CAPACITY] = {0};
    memcpy(captured_uri, uri, bytes + 1U);
    size_t active = active_index(coordinator);
    if (active != SIZE_MAX)
        memcpy(previous, coordinator->entries[active].view_id,
               strlen(coordinator->entries[active].view_id) + 1U);
    UmiDocumentId target = 0U, created = 0U;
    UmiStatus status = UmiDocumentCoordinatorFindSourceUri(coordinator, captured_uri, &target);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        status = UmiDocumentLocalFileUriToPath(captured_uri, path, sizeof(path));
        if (status == UMI_STATUS_OK)
            status = SourceNavigationFindPath(coordinator, path, &target);
    }
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
        return status;
    UmiEditorSourceLocation departure;
    UmiStatus captured = NavigationCapture(coordinator, &departure);
    ++coordinator->navigation_depth;
    UmiDocumentLoadResult loaded = {0};
    if (status == UMI_STATUS_NOT_FOUND)
    {
        if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES ||
            umi_ui_document_view_model_count(umi_ui_workbench_documents(coordinator->workbench)) >=
                UMI_UI_DOCUMENT_VIEW_MAX)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            UmiDocumentLoadOptions options = umi_document_load_options_default();
            options.maximum_bytes = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
            options.normalise_to = UMI_DOCUMENT_LINE_ENDING_LF;
            status = umi_document_load(&coordinator->provider, path, &options, &loaded);
        }
        if (status == UMI_STATUS_OK)
        {
            /* A provider callback may have opened this file while reading.
             * Prefer its live draft and never overwrite that new working copy. */
            UmiStatus existing = SourceNavigationFindPath(coordinator, path, &target);
            if (existing == UMI_STATUS_NOT_FOUND)
            {
                size_t first = 0U, last = 0U;
                status = SourceNavigationOffsets(loaded.text, loaded.text_length, start, end, &first, &last);
                if (status == UMI_STATUS_OK)
                {
                    char view_id[UMI_UI_ID_CAPACITY];
                    status = AttachLoadedDocument(coordinator, path, &loaded, view_id, sizeof(view_id));
                    if (status == UMI_STATUS_OK)
                    {
                        size_t index = find_view(coordinator, view_id);
                        if (index == SIZE_MAX)
                            status = UMI_STATUS_INVALID_STATE;
                        else
                            target = created = coordinator->entries[index].document_id;
                    }
                }
            }
            else if (existing != UMI_STATUS_OK)
                status = existing;
        }
    }
    size_t offset = 0U, selection = 0U;
    if (status == UMI_STATUS_OK)
        status = SourceNavigationSelect(coordinator, target, start, end, &offset, &selection);
    umi_document_load_result_dispose(&loaded);
    if (status != UMI_STATUS_OK)
    {
        if (created != 0U)
            SourceNavigationDiscard(coordinator, created);
        if (previous[0] != '\0' && find_view(coordinator, previous) != SIZE_MAX)
            (void)umi_ui_workbench_activate_document(coordinator->workbench, previous);
    }
    --coordinator->navigation_depth;
    if (status == UMI_STATUS_OK)
    {
        NavigationRecord(coordinator, &departure, captured);
        if (out_document != NULL)
            *out_document = target;
        if (out_offset != NULL)
            *out_offset = offset;
        if (out_bytes != NULL)
            *out_bytes = selection;
    }
    return status;
}
#endif
UmiStatus UmiDocumentCoordinatorOpenSourceRange(UmiDocumentCoordinator *coordinator, const char *uri,
                                                UmiEditorTextPosition start, UmiEditorTextPosition end,
                                                UmiDocumentId *out_document, size_t *out_offset,
                                                size_t *out_bytes)
{
    if (coordinator == NULL || uri == NULL || uri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t bytes = 0U;
    while (bytes <= 8192U && uri[bytes] != '\0')
        ++bytes;
    if (bytes > 8192U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char captured_uri[8193], path[UMI_PATH_CAPACITY], previous[UMI_UI_ID_CAPACITY] = {0};
    memcpy(captured_uri, uri, bytes + 1U);
    size_t active = active_index(coordinator);
    if (active != SIZE_MAX)
        memcpy(previous, coordinator->entries[active].view_id,
               strlen(coordinator->entries[active].view_id) + 1U);
    UmiDocumentId target = 0U, created = 0U;
    UmiStatus status = UmiDocumentCoordinatorFindSourceUri(coordinator, captured_uri, &target);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        status = UmiDocumentLocalFileUriToPath(captured_uri, path, sizeof(path));
        if (status == UMI_STATUS_OK)
            status = SourceNavigationFindPath(coordinator, path, &target);
    }
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
        return status;
    UmiEditorSourceLocation departure;
    UmiStatus captured = NavigationCapture(coordinator, &departure);
    ++coordinator->navigation_depth;
    UmiDocumentLoadResult loaded = {0};
    if (status == UMI_STATUS_NOT_FOUND)
    {
        if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES ||
            umi_ui_document_view_model_count(umi_ui_workbench_documents(coordinator->workbench)) >=
                UMI_UI_DOCUMENT_VIEW_MAX)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            UmiDocumentLoadOptions options = umi_document_load_options_default();
            options.maximum_bytes = UMI_DOCUMENT_COORDINATOR_MAXIMUM_FILE_BYTES;
            options.normalise_to = UMI_DOCUMENT_LINE_ENDING_LF;
            status = umi_document_load(&coordinator->provider, path, &options, &loaded);
        }
        if (status == UMI_STATUS_OK)
        {
            /* A provider callback may have opened this file while reading.
             * Prefer its live draft and never overwrite that new working copy. */
            UmiStatus existing = SourceNavigationFindPath(coordinator, path, &target);
            if (existing == UMI_STATUS_NOT_FOUND)
            {
                size_t first = 0U, last = 0U;
                status = SourceNavigationOffsets(loaded.text, loaded.text_length, start, end, &first, &last);
                if (status == UMI_STATUS_OK)
                {
                    char view_id[UMI_UI_ID_CAPACITY];
                    status = AttachLoadedDocument(coordinator, path, &loaded, view_id, sizeof(view_id));
                    if (status == UMI_STATUS_OK)
                    {
                        size_t index = find_view(coordinator, view_id);
                        if (index == SIZE_MAX)
                            status = UMI_STATUS_INVALID_STATE;
                        else
                            target = created = coordinator->entries[index].document_id;
                    }
                }
            }
            else if (existing != UMI_STATUS_OK)
                status = existing;
        }
    }
    size_t offset = 0U, selection = 0U;
    if (status == UMI_STATUS_OK)
        status = SourceNavigationSelect(coordinator, target, start, end, &offset, &selection);
    umi_document_load_result_dispose(&loaded);
    if (status != UMI_STATUS_OK)
    {
        if (created != 0U)
            SourceNavigationDiscard(coordinator, created);
        if (previous[0] != '\0' && find_view(coordinator, previous) != SIZE_MAX)
            (void)umi_ui_workbench_activate_document(coordinator->workbench, previous);
    }
    --coordinator->navigation_depth;
    if (status == UMI_STATUS_OK)
    {
        NavigationRecord(coordinator, &departure, captured);
        if (out_document != NULL)
            *out_document = target;
        if (out_offset != NULL)
            *out_offset = offset;
        if (out_bytes != NULL)
            *out_bytes = selection;
    }
    return status;
}

/* Diagnostic ranges belong to a particular source snapshot. Validate that proof
 * through the live view owner before reusing the normal navigation transaction;
 * filenames alone cannot establish freshness after an unsaved edit. */
UmiStatus UmiDocumentCoordinatorSelectCapturedSourceRange(
    UmiDocumentCoordinator *coordinator, UmiDocumentId document,
    const char *captured_uri, const char *captured_source, size_t source_bytes,
    UmiEditorTextPosition start, UmiEditorTextPosition end,
    size_t *out_offset, size_t *out_bytes)
{
    if (coordinator == NULL || document == 0U || captured_uri == NULL ||
        captured_uri[0] == '\0' || captured_source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = EditDocumentIndex(coordinator, document);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot current;
    UmiStatus status = umi_ui_document_view_model_find(
        views, coordinator->entries[index].view_id, &current);
    if (status == UMI_STATUS_OK && strcmp(current.uri, captured_uri) != 0)
        status = UMI_STATUS_INVALID_STATE;
    char *source = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentViewModelCopyText(views, current.view_id, &source, &bytes);
    if (status == UMI_STATUS_OK &&
        (bytes != source_bytes || memcmp(source, captured_source, bytes) != 0))
        status = UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModelFreeText(source);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorSelectSourceRange(
            coordinator, document, start, end, out_offset, out_bytes);
    return status;
}
