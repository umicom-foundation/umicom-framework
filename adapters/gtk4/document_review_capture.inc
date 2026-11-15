/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_review_capture.inc
 *
 * PURPOSE:
 *   Capture and validate a source draft for native review admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The caller owns the returned request on success. Admission is shared by
 * menu queries and every existing source review; it never reads preferences,
 * saves a file, publishes a window or starts a language server. */
static UmiStatus CompletionReviewCapture(
    UmiGtk4Adapter *adapter, int inspection, int single_line,
    UmiDocumentSourceRequest **out_source,
    UmiDocumentSourceRequestSummary *out_summary)
{
    if (out_source == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_source = NULL;
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL ||
        adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterCompletionReviewBusy(adapter) ||
        UmiGtk4AdapterDocumentProposalBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || adapter->edit_cancel != NULL)
        return UMI_STATUS_BUSY;

    UmiDocumentId id = 0U;
    UmiDocumentSourceRequest *source = NULL;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status == UMI_STATUS_OK)
        status = inspection
            ? UmiDocumentSourceRequestCreateInspection(adapter->edit_coordinator, id, &source)
            : UmiDocumentSourceRequestCreate(adapter->edit_coordinator, id, &source);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentSourceRequestSummary summary;
    const char *text = NULL;
    size_t bytes = 0U;
    status = UmiDocumentSourceRequestInspect(source, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestRead(source, &text, &bytes);
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorTextPosition begin, end;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, summary.cursor_offset, &begin);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(
            &view, summary.cursor_offset + summary.selection_bytes, &end);
    if (status == UMI_STATUS_OK && single_line && begin.line != end.line)
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status != UMI_STATUS_OK) {
        UmiDocumentSourceRequestDestroy(source);
        return status;
    }
    *out_summary = summary;
    *out_source = source;
    return UMI_STATUS_OK;
}
