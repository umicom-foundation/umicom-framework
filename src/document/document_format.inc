/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/document_format.inc
 * PURPOSE: Publish source conversion and next-save format in one document history operation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDocumentCoordinatorSetFormat(UmiDocumentCoordinator *coordinator,
                                          const UmiDocumentFormatOptions *options)
{
    if (coordinator == NULL || options == NULL ||
        (options->ensure_final_newline != 0 && options->ensure_final_newline != 1) ||
        options->encoding < UMI_DOCUMENT_ENCODING_UTF8 || options->encoding > UMI_DOCUMENT_ENCODING_UNKNOWN ||
        options->encoding == UMI_DOCUMENT_ENCODING_BINARY ||
        options->line_ending < UMI_DOCUMENT_LINE_ENDING_NONE ||
        options->line_ending > UMI_DOCUMENT_LINE_ENDING_CR ||
        (options->line_ending == UMI_DOCUMENT_LINE_ENDING_NONE && options->ensure_final_newline))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, entry->view_id, &before);
    if (status != UMI_STATUS_OK)
        return status;
    if (before.read_only)
        return UMI_STATUS_PERMISSION_DENIED;
    char identity[UMI_UI_ID_CAPACITY];
    (void)snprintf(identity, sizeof(identity), "document.%llu", (unsigned long long)entry->document_id);
    if (strcmp(before.document_id, identity) != 0)
        return UMI_STATUS_INVALID_STATE;
    /* Resolve keep-current choices once. Encoding-only changes retain the
     * exact visible bytes, even if the current buffer has mixed endings. */
    UmiDocumentFormatOptions effective = *options;
    if (effective.encoding == UMI_DOCUMENT_ENCODING_UNKNOWN)
        effective.encoding = entry->encoding;
    if (effective.line_ending == UMI_DOCUMENT_LINE_ENDING_NONE)
        effective.line_ending = entry->line_ending;
    char *source = NULL;
    size_t bytes = 0U;
    status = UmiUiDocumentViewModelCopyText(views, before.view_id, &source, &bytes);
    UmiDocumentFormatPlan *plan = NULL;
    UmiDocumentFormatSummary summary;
    const char *proposed = NULL;
    size_t proposed_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status =
            UmiDocumentFormatPlanCreate(source, bytes, before.cursor_offset, before.selection_length,
                                        options->line_ending, options->ensure_final_newline, NULL, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentFormatPlanInspect(plan, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentFormatPlanRead(plan, &proposed, &proposed_bytes);
    if (status == UMI_STATUS_OK)
    {
        UmiUiDocumentViewSnapshot after = before;
        after.cursor_offset = summary.cursor_offset;
        after.selection_length = summary.selection_bytes;
        /* Encoding chooses the next Save representation. The live editor stays
         * UTF-8, and this transaction marks metadata-only changes as dirty. */
        status = CommitViewTextWithFormat(coordinator, index, &before, &after, source, bytes, proposed,
                                          proposed_bytes, &effective);
    }
    UmiDocumentFormatPlanDestroy(plan);
    UmiUiDocumentViewModelFreeText(source);
    return status;
}
