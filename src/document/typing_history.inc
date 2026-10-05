/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/typing_history.inc
 * PURPOSE: Commit a native typing group with its exact departure selection and existing document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDocumentCoordinatorCommitTyping(UmiDocumentCoordinator *coordinator,
                                             UmiDocumentEditPlan *capture)
{
    if (coordinator == NULL || capture == NULL || capture->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capture->consumed)
        return UMI_STATUS_INVALID_STATE;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = EditDocumentIndex(coordinator, capture->stored.document_id);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot current;
    UmiDocumentSnapshot stored;
    UmiStatus status = umi_document_store_snapshot(coordinator->store, capture->stored.document_id, &stored);
    if (status == UMI_STATUS_OK)
        status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &current);
    if (status != UMI_STATUS_OK)
        return status;
    if (current.read_only || capture->view.read_only)
        return UMI_STATUS_PERMISSION_DENIED;
    /* The visible text is expected to change during typing. Its document,
     * saved identity and underlying store must still be the captured owner. */
    if (stored.revision != capture->stored.revision ||
        stored.saved_revision != capture->stored.saved_revision ||
        stored.has_path != capture->stored.has_path || strcmp(stored.path, capture->stored.path) != 0 ||
        strcmp(current.view_id, capture->view.view_id) != 0 ||
        strcmp(current.document_id, capture->view.document_id) != 0 ||
        strcmp(current.uri, capture->view.uri) != 0)
        return UMI_STATUS_INVALID_STATE;
    char *previous = NULL, *draft = NULL;
    size_t previous_bytes = 0U, draft_bytes = 0U;
    status = copy_store_text(coordinator, index, &previous, &previous_bytes);
    if (status == UMI_STATUS_OK &&
        (previous_bytes != capture->length || memcmp(previous, capture->text, previous_bytes) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentViewModelCopyText(views, current.view_id, &draft, &draft_bytes);
    if (status == UMI_STATUS_OK &&
        (memchr(draft, '\0', draft_bytes) != NULL ||
         !umi_document_utf8_validate((const unsigned char *)draft, draft_bytes, NULL) ||
         !EditSelectionValid(&current, draft, draft_bytes)))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        /* CommitViewText reserves projection storage before publishing and
         * retains the departure selection beside the exact old source. */
        status = CommitViewText(coordinator, index, &capture->view, &current, capture->text, capture->length,
                                draft, draft_bytes);
        if (status == UMI_STATUS_OK)
            capture->consumed = 1;
    }
    umi_document_store_free_text(previous);
    UmiUiDocumentViewModelFreeText(draft);
    return status;
}
