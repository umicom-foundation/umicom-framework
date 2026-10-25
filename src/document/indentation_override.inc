/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/indentation_override.inc
 *
 * PURPOSE:
 *   Keep checked indentation overrides and their revisions with the existing document owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static int IndentationSame(const UmiEditorIndentation *a, const UmiEditorIndentation *b) {
    return a->tab_width == b->tab_width && a->insert_spaces == b->insert_spaces;
}
UmiStatus UmiDocumentCoordinatorInspectIndentation(const UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentIndentationReview *out) {
    if (coordinator == NULL || documentId == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(coordinator->workbench), entry->view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    char identity[UMI_UI_ID_CAPACITY];
    (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)documentId);
    if (strcmp(view.document_id, identity) != 0) return UMI_STATUS_INVALID_STATE;
    UmiDocumentIndentationReview result = {0};
    result.owner = coordinator;
    result.document_id = documentId;
    result.defaults_revision = coordinator->indentation_revision;
    result.override_revision = entry->indentation_revision;
    result.defaults = coordinator->indentation;
    result.overridden = entry->has_indentation_override;
    result.effective = result.overridden ? entry->indentation_override : result.defaults;
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorCheckIndentation(const UmiDocumentCoordinator *coordinator,
    const UmiDocumentIndentationReview *review) {
    if (coordinator == NULL || review == NULL || review->document_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->owner != coordinator) return UMI_STATUS_INVALID_STATE;
    UmiDocumentIndentationReview current;
    UmiStatus status = UmiDocumentCoordinatorInspectIndentation(coordinator, review->document_id, &current);
    if (status != UMI_STATUS_OK) return status;
    if (current.defaults_revision != review->defaults_revision ||
        current.override_revision != review->override_revision ||
        current.overridden != review->overridden ||
        !IndentationSame(&current.defaults, &review->defaults) ||
        !IndentationSame(&current.effective, &review->effective))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorApplyIndentation(UmiDocumentCoordinator *coordinator,
    const UmiDocumentIndentationReview *review, const UmiEditorIndentation *override,
    UmiDocumentIndentationReview *out) {
    if (override != NULL && UmiEditorIndentationValidate(override) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDocumentCoordinatorCheckIndentation(coordinator, review);
    if (status != UMI_STATUS_OK) return status;
    UmiDocumentIndentationReview next = *review;
    UmiDocumentCoordinatorEntry *entry =
        &coordinator->entries[EditDocumentIndex(coordinator, review->document_id)];
    const int overridden = override != NULL;
    const int changed = entry->has_indentation_override != overridden ||
        (overridden && !IndentationSame(&entry->indentation_override, override));
    if (changed) {
        if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
            coordinator->navigation_depth != 0U) return UMI_STATUS_BUSY;
        if (entry->indentation_revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* A working copy owns the override. The session preference stays shared,
         * but project-specific choices cannot accidentally affect other tabs. */
        entry->has_indentation_override = overridden;
        entry->indentation_override = override != NULL ? *override : coordinator->indentation;
        ++entry->indentation_revision;
        next.overridden = overridden;
        next.override_revision = entry->indentation_revision;
        next.effective = entry->indentation_override;
    }
    if (out != NULL) *out = next;
    return UMI_STATUS_OK;
}
