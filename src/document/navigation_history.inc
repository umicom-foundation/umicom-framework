/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/navigation_history.inc
 * PURPOSE: Record document jumps and apply history only after resolving a live destination.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


/* Capture the full draft so positions past the small presentation preview are
 * correct. The view ID identifies this open document even after Save As. */
/* History and bookmarks capture byte columns with the same LF, CRLF and CR rules used when restoring them.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus NavigationCapture(const UmiDocumentCoordinator *coordinator,
    UmiEditorSourceLocation *out_location)
{
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &view);
    char *text = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views, view.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    size_t offset = view.cursor_offset < length ? view.cursor_offset : length;
    while (offset > 0U && offset < length && ((unsigned char)text[offset] & 0xc0U) == 0x80U) --offset;
    uint64_t line = 1U, column = 1U;
    for (size_t byte = 0U; byte < offset; ++byte) {
        if (text[byte] == '\n') { ++line; column = 1U; }
        else ++column;
    }
    UmiUiDocumentViewModelFreeText(text);
    return umi_editor_source_location_initialize(out_location, view.view_id, line, column);
}
#endif
static UmiStatus NavigationCapture(const UmiDocumentCoordinator *coordinator,
    UmiEditorSourceLocation *out_location)
{
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &view);
    char *text = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views, view.view_id, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    uint64_t line=1U,column=1U;
    NavigationTextLocation(text,length,view.cursor_offset,&line,&column);
    UmiUiDocumentViewModelFreeText(text);
    return umi_editor_source_location_initialize(out_location, view.view_id, line, column);
}

/* Optional location bookkeeping must not turn a successful source operation
 * into a reported failure. Expose capture errors separately for host status UI.
 * Recording a first document seeds the timeline without inventing a departure. */
static void NavigationRecord(UmiDocumentCoordinator *coordinator,
    const UmiEditorSourceLocation *departure, UmiStatus captured)
{
    UmiEditorSourceLocation destination;
    UmiStatus status = NavigationCapture(coordinator, &destination);
    if (status == UMI_STATUS_OK && captured != UMI_STATUS_OK && captured != UMI_STATUS_NOT_FOUND)
        status = captured;
    if (status == UMI_STATUS_OK) status = captured == UMI_STATUS_OK
        ? UmiEditorNavigationHistoryRecordJump(coordinator->navigation_history, departure, &destination)
        : umi_editor_navigation_history_record(coordinator->navigation_history, &destination);
    coordinator->navigation_record_status = status;
}

UmiStatus UmiDocumentCoordinatorNavigationSnapshot(const UmiDocumentCoordinator *coordinator,
    UmiDocumentNavigationHistorySnapshot *out_snapshot)
{
    if (coordinator == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorNavigationHistorySnapshot history;
    UmiStatus status = umi_editor_navigation_history_snapshot(coordinator->navigation_history, &history);
    if (status != UMI_STATUS_OK) return status;
    UmiDocumentNavigationHistorySnapshot snapshot = {0};
    snapshot.count = history.count; snapshot.revision = history.revision;
    snapshot.can_go_back = history.can_go_back; snapshot.can_go_forward = history.can_go_forward;
    snapshot.busy = coordinator->save_in_progress || coordinator->reopen_in_progress || coordinator->navigation_depth != 0U;
    snapshot.last_record_status = coordinator->navigation_record_status;
    *out_snapshot = snapshot;
    return UMI_STATUS_OK;
}

/* Resolve before changing the active document. Text edits since the visit may
 * remove its line; returning NOT_FOUND then preserves both tab and timeline. */
/* History and bookmark travel resolve mixed line endings before changing selection or the active document.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus NavigationApply(void *context, const UmiEditorSourceLocation *destination)
{
    UmiDocumentCoordinator *coordinator = context;
    size_t index = 0U;
    while (index < coordinator->count && strcmp(coordinator->entries[index].view_id, destination->uri) != 0) ++index;
    if (index == coordinator->count) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, destination->uri, &before);
    char *text = NULL;
    size_t length = 0U, offset = 0U;
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views, destination->uri, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    uint64_t line = 1U;
    while (offset < length && line < destination->line) if (text[offset++] == '\n') ++line;
    if (line != destination->line) { UmiUiDocumentViewModelFreeText(text); return UMI_STATUS_NOT_FOUND; }
    size_t end = offset;
    while (end < length && text[end] != '\n' && text[end] != '\r') ++end;
    uint64_t delta = destination->column > 0U ? destination->column - 1U : 0U;
    size_t start = offset;
    offset += delta < (uint64_t)(end - offset) ? (size_t)delta : end - offset;
    while (offset > start && offset < length && ((unsigned char)text[offset] & 0xc0U) == 0x80U) --offset;
    UmiUiDocumentViewModelFreeText(text);
    UmiUiDocumentViewSnapshot after = before;
    after.cursor_offset = offset; after.selection_length = 0U;
    status = umi_ui_document_view_model_upsert(views, &after);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_activate_document(coordinator->workbench, after.view_id);
    if (status != UMI_STATUS_OK) (void)umi_ui_document_view_model_upsert(views, &before);
    return status;
}
#endif
static UmiStatus NavigationApply(void *context, const UmiEditorSourceLocation *destination)
{
    UmiDocumentCoordinator *coordinator = context;
    size_t index = 0U;
    while (index < coordinator->count && strcmp(coordinator->entries[index].view_id, destination->uri) != 0) ++index;
    if (index == coordinator->count) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, destination->uri, &before);
    char *text = NULL;
    size_t length = 0U, offset = 0U;
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelCopyText(views, destination->uri, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    status=NavigationTextOffset(text,length,destination->line,destination->column,&offset);
    UmiUiDocumentViewModelFreeText(text);
    if(status!=UMI_STATUS_OK) return status;
    UmiUiDocumentViewSnapshot after = before;
    after.cursor_offset = offset; after.selection_length = 0U;
    status = umi_ui_document_view_model_upsert(views, &after);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_activate_document(coordinator->workbench, after.view_id);
    if (status != UMI_STATUS_OK) (void)umi_ui_document_view_model_upsert(views, &before);
    return status;
}

static UmiStatus NavigationReady(UmiDocumentCoordinator *coordinator, uint64_t revision)
{
    UmiDocumentNavigationHistorySnapshot snapshot;
    UmiStatus status = UmiDocumentCoordinatorNavigationSnapshot(coordinator, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.busy) return UMI_STATUS_BUSY;
    if (revision != snapshot.revision) return UMI_STATUS_INVALID_STATE;
    return snapshot.revision == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorTravel(UmiDocumentCoordinator *coordinator,
    int direction, uint64_t expected_revision, UmiDocumentId *out_document)
{
    UmiStatus status = NavigationReady(coordinator, expected_revision);
    if (status != UMI_STATUS_OK) return status;
    if (direction != -1 && direction != 1) return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorSourceLocation departure;
    status = NavigationCapture(coordinator, &departure);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    ++coordinator->navigation_depth;
    status = UmiEditorNavigationHistoryTravel(coordinator->navigation_history, direction,
        expected_revision, status == UMI_STATUS_OK ? &departure : NULL, NavigationApply, coordinator, NULL);
    --coordinator->navigation_depth;
    if (status == UMI_STATUS_OK && out_document != NULL)
        *out_document = coordinator->entries[active_index(coordinator)].document_id;
    return status;
}

UmiStatus UmiDocumentCoordinatorClearNavigation(UmiDocumentCoordinator *coordinator,
    uint64_t expected_revision)
{
    UmiStatus status = NavigationReady(coordinator, expected_revision);
    if (status == UMI_STATUS_OK) status = umi_editor_navigation_history_clear(coordinator->navigation_history);
    if (status == UMI_STATUS_OK) coordinator->navigation_record_status = UMI_STATUS_OK;
    return status;
}
