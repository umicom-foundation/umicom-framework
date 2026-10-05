/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/recovery_coordinator.inc
 * PURPOSE: Reuse the authoritative store/view bridge for private recovery without taking ownership of saved paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_coordinator.h"
#include "umicom/base/text.h"
UmiStatus UmiDocumentCoordinatorCaptureRecovery(UmiDocumentCoordinator *coordinator, UmiDocumentId document,
                                                const char *key, const UmiCancellationToken *cancel,
                                                UmiDocumentRecoveryDraft **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (coordinator == NULL || key == NULL || document == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(key) != 32U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    size_t index = EditDocumentIndex(coordinator, document);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentSnapshot stored;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentRecoveryInfo info = {0};
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_document_store_snapshot(coordinator->store, document, &stored);
    if (status == UMI_STATUS_OK)
        status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &view);
    char *source = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
    {
        char identity[UMI_UI_ID_CAPACITY];
        (void)snprintf(identity, sizeof(identity), "document.%llu", (unsigned long long)document);
        if (strcmp(view.document_id, identity) != 0)
            return UMI_STATUS_INVALID_STATE;
        status = UmiUiDocumentViewModelCopyText(views, view.view_id, &source, &bytes);
    }
    if (status == UMI_STATUS_OK)
    {
        memcpy(info.key, key, sizeof(info.key));
        status = umi_text_copy(info.display_name, sizeof(info.display_name), stored.display_name);
        if (status == UMI_STATUS_OK && stored.has_path)
            status = umi_text_copy(info.source_path, sizeof(info.source_path), stored.path);
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(info.language_id, sizeof(info.language_id), view.language_id);
        info.source_revision = stored.revision;
        info.text_bytes = bytes;
        info.cursor_offset = view.cursor_offset;
        info.selection_bytes = view.selection_length;
        if (status == UMI_STATUS_OK)
            status = UmiDocumentRecoveryDraftCreate(&info, source, bytes, cancel, out);
    }
    UmiUiDocumentViewModelFreeText(source);
    return status;
}
UmiStatus UmiDocumentCoordinatorRestoreRecovery(UmiDocumentCoordinator *coordinator,
                                                const UmiDocumentRecoveryDraft *draft, UmiDocumentId *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = 0U;
    if (coordinator == NULL || draft == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    if (coordinator->count >= UMI_DOCUMENT_MAX_WORKING_COPIES ||
        umi_ui_document_view_model_count(views) >= UMI_UI_DOCUMENT_VIEW_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentRecoveryInfo info;
    const char *source = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiDocumentRecoveryDraftInspect(draft, &info);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryDraftRead(draft, &source, &bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (strlen(info.display_name) >= UMI_DOCUMENT_DISPLAY_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentLineEndingAnalysis endings;
    status = umi_document_line_endings_analyse(source, bytes, &endings);
    UmiDocumentId document = 0U;
    size_t index = SIZE_MAX;
    if (status == UMI_STATUS_OK)
        status = umi_document_store_new(coordinator->store, info.display_name, &document);
    if (status == UMI_STATUS_OK)
        status = umi_document_store_replace_text(coordinator->store, document, source, bytes);
    if (status == UMI_STATUS_OK)
        status = register_entry(coordinator, document, UMI_DOCUMENT_ENCODING_UTF8, endings.dominant, NULL,
                                NULL, &index);
    UmiUiDocumentViewSnapshot view = {0}, existing;
    if (status == UMI_STATUS_OK)
    {
        status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &existing);
        if (status == UMI_STATUS_OK)
            status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK)
    {
        /* Publish a complete owned draft in one operation. The path remains
         * empty in DocumentStore; the descriptive recovery path is never bound
         * to this working copy. New contributors should preserve this boundary. */
        (void)snprintf(view.view_id, sizeof(view.view_id), "%s", coordinator->entries[index].view_id);
        (void)snprintf(view.document_id, sizeof(view.document_id), "document.%llu",
                       (unsigned long long)document);
        (void)snprintf(view.uri, sizeof(view.uri), "untitled:///recovered-%s", info.key);
        status = umi_text_copy(view.title, sizeof(view.title), info.display_name);
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(view.language_id, sizeof(view.language_id), info.language_id);
        view.dirty = 1;
        view.pinned = 1;
        view.closable = 1;
        view.cursor_offset = info.cursor_offset;
        view.selection_length = info.selection_bytes;
        if (status == UMI_STATUS_OK)
            status = UmiUiDocumentViewModelUpsertText(views, &view, source, bytes);
    }
    if (status == UMI_STATUS_OK)
    {
        /* Activation of a just-published valid view does not allocate. No
         * external provider callbacks occur between publication and activation. */
        status = umi_ui_workbench_activate_document(coordinator->workbench, view.view_id);
        if (status == UMI_STATUS_OK)
        {
            *out = document;
            return status;
        }
        (void)umi_ui_document_view_model_remove(views, view.view_id);
    }
    if (index != SIZE_MAX)
    {
        entry_dispose(&coordinator->entries[index]);
        --coordinator->count;
    }
    if (document != 0U)
        (void)umi_document_store_close(coordinator->store, document, 1);
    return status;
}
