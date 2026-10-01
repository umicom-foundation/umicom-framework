/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/replacement_review.inc
 * PURPOSE: Keep reviewed replacements bound to the captured draft and existing undo owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included by coordinator.c after its edit helpers. The plan reuses the same
 * document capture and CommitViewText transaction as other editor actions;
 * applications only present the two immutable buffers and request Apply. */
struct UmiDocumentReplacementPlan {
    UmiDocumentEditPlan *capture;
    UmiDocumentReplacementSummary summary;
    char *proposed;
    UmiDocumentFingerprint baseline;
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding lineEnding;
    UmiDocumentConflictState conflict;
    int consumed;
};

/* A review owns its text, never its coordinator or native widgets. */
void UmiDocumentReplacementPlanDestroy(UmiDocumentReplacementPlan *plan)
{
    if (plan == NULL) return;
    UmiDocumentEditPlanDestroy(plan->capture);
    free(plan->proposed);
    free(plan);
}

/* Bound input scans before size arithmetic or expensive matching. */
static UmiStatus ReplacementInputLength(const char *text, size_t *outLength)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length <= UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES && text[length] != '\0') ++length;
    if (length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_document_utf8_validate((const unsigned char *)text, length, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    *outLength = length;
    return UMI_STATUS_OK;
}

/* Preparation calculates all bytes before publishing a review to its caller. */
UmiStatus UmiDocumentCoordinatorPrepareReplacement(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, const char *needle, const char *replacement,
    UmiDocumentReplacementPlan **outPlan)
{
    if (outPlan != NULL) *outPlan = NULL;
    if (coordinator == NULL || documentId == 0U || outPlan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t needleBytes = 0U, replacementBytes = 0U;
    UmiStatus status = ReplacementInputLength(needle, &needleBytes);
    if (status == UMI_STATUS_OK) status = ReplacementInputLength(replacement, &replacementBytes);
    if (status != UMI_STATUS_OK) return status;
    if (needleBytes == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiDocumentReplacementPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiDocumentCoordinatorPrepareEdit(coordinator, documentId, &plan->capture);
    if (status == UMI_STATUS_OK && plan->capture->view.read_only) status = UMI_STATUS_PERMISSION_DENIED;
    /* The generic view model can hold arbitrary non-NUL bytes. A text review
     * must not display invalid encoding or cut through a malformed sequence. */
    if (status == UMI_STATUS_OK && !umi_document_utf8_validate(
            (const unsigned char *)plan->capture->text, plan->capture->length, NULL))
        status = UMI_STATUS_INVALID_STATE;
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0};
    if (status == UMI_STATUS_OK)
        status = UmiEditorSearchReplaceAllSize(plan->capture->text, plan->capture->length,
            needle, needleBytes, replacementBytes, &options,
            &plan->summary.proposed_bytes, &plan->summary.match_count);
    if (status == UMI_STATUS_OK && plan->summary.proposed_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        plan->proposed = malloc(plan->summary.proposed_bytes + 1U);
        if (plan->proposed == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
        status = UmiEditorSearchReplaceAll(plan->capture->text, plan->capture->length,
            needle, needleBytes, replacement, replacementBytes, &options,
            plan->proposed, plan->summary.proposed_bytes + 1U, NULL);
    if (status != UMI_STATUS_OK) { UmiDocumentReplacementPlanDestroy(plan); return status; }
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    plan->baseline = entry->baseline;
    plan->encoding = entry->encoding;
    plan->lineEnding = entry->line_ending;
    plan->conflict = entry->conflict;
    plan->summary.document_id = documentId;
    (void)snprintf(plan->summary.display_name, sizeof plan->summary.display_name,
        "%s", plan->capture->stored.display_name);
    plan->summary.previous_bytes = plan->capture->length;
    plan->summary.text_changes = plan->capture->length != plan->summary.proposed_bytes ||
        memcmp(plan->capture->text, plan->proposed, plan->capture->length) != 0;
    UmiDocumentWorkingCopySnapshot snapshot;
    status = snapshot_index(coordinator, index, &snapshot);
    if (status != UMI_STATUS_OK) { UmiDocumentReplacementPlanDestroy(plan); return status; }
    plan->summary.has_unsaved_changes = snapshot.dirty;
    *outPlan = plan;
    return UMI_STATUS_OK;
}

/* Readers see copies or borrowed immutable bytes without touching services. */
UmiStatus UmiDocumentReplacementPlanSummary(const UmiDocumentReplacementPlan *plan,
    UmiDocumentReplacementSummary *outSummary)
{
    if (plan == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    *outSummary = plan->summary;
    return UMI_STATUS_OK;
}

/* Clear partial output arguments even when the complete call is invalid. */
UmiStatus UmiDocumentReplacementPlanTexts(const UmiDocumentReplacementPlan *plan,
    const char **outPrevious, size_t *outPreviousBytes,
    const char **outProposed, size_t *outProposedBytes)
{
    if (outPrevious != NULL) *outPrevious = NULL;
    if (outPreviousBytes != NULL) *outPreviousBytes = 0U;
    if (outProposed != NULL) *outProposed = NULL;
    if (outProposedBytes != NULL) *outProposedBytes = 0U;
    if (plan == NULL || outPrevious == NULL || outPreviousBytes == NULL ||
        outProposed == NULL || outProposedBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    *outPrevious = plan->capture->text;
    *outPreviousBytes = plan->summary.previous_bytes;
    *outProposed = plan->proposed;
    *outProposedBytes = plan->summary.proposed_bytes;
    return UMI_STATUS_OK;
}

/* Compare both revisions and bytes so a replaced view with reused metadata
 * cannot apply a review to a different draft. Selection alone is irrelevant
 * to a whole-document edit and the latest caret is kept when possible. */
static UmiStatus ReplacementCurrent(UmiDocumentCoordinator *coordinator,
    const UmiDocumentReplacementPlan *plan, size_t *outIndex,
    UmiUiDocumentViewSnapshot *outView)
{
    if (coordinator == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->consumed) return UMI_STATUS_INVALID_STATE;
    const UmiDocumentEditPlan *capture = plan->capture;
    if (capture->owner != coordinator) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, capture->stored.document_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiDocumentSnapshot stored;
    UmiUiDocumentTextInfo textInfo;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiStatus status = umi_document_store_snapshot(coordinator->store, capture->stored.document_id, &stored);
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_find(views,
        coordinator->entries[index].view_id, outView);
    if (status == UMI_STATUS_OK) status = UmiUiDocumentViewModelTextInfo(views, outView->view_id, &textInfo);
    if (status != UMI_STATUS_OK) return status;
    if (outView->read_only) return UMI_STATUS_PERMISSION_DENIED;
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    if (stored.revision != capture->stored.revision || stored.saved_revision != capture->stored.saved_revision ||
        stored.has_path != capture->stored.has_path || strcmp(stored.path, capture->stored.path) != 0 ||
        strcmp(stored.display_name, capture->stored.display_name) != 0 ||
        stored.external_change != capture->stored.external_change || stored.dirty != capture->stored.dirty ||
        textInfo.text_revision != capture->textInfo.text_revision || textInfo.byte_count != capture->length ||
        strcmp(outView->view_id, capture->view.view_id) != 0 ||
        strcmp(outView->document_id, capture->view.document_id) != 0 ||
        strcmp(outView->uri, capture->view.uri) != 0 || outView->dirty != capture->view.dirty ||
        entry->encoding != plan->encoding || entry->line_ending != plan->lineEnding || entry->conflict != plan->conflict ||
        entry->baseline.valid != plan->baseline.valid ||
        (entry->baseline.valid && !umi_document_fingerprint_equal(&entry->baseline, &plan->baseline)))
        return UMI_STATUS_INVALID_STATE;
    char *current = NULL;
    size_t currentBytes = 0U;
    status = UmiUiDocumentViewModelCopyText(views, outView->view_id, &current, &currentBytes);
    if (status == UMI_STATUS_OK && (currentBytes != capture->length ||
        memcmp(current, capture->text, currentBytes) != 0)) status = UMI_STATUS_INVALID_STATE;
    UmiUiDocumentViewModelFreeText(current);
    if (status == UMI_STATUS_OK) *outIndex = index;
    return status;
}

/* A check never refreshes the plan into a different question. */
UmiStatus UmiDocumentCoordinatorCheckReplacement(UmiDocumentCoordinator *coordinator,
    const UmiDocumentReplacementPlan *plan)
{
    size_t index = 0U;
    UmiUiDocumentViewSnapshot current;
    return ReplacementCurrent(coordinator, plan, &index, &current);
}

/* Commit through the shared history transaction after checking the original
 * target. No provider callback or file write occurs between check and commit. */
UmiStatus UmiDocumentCoordinatorApplyReplacement(UmiDocumentCoordinator *coordinator,
    UmiDocumentReplacementPlan *plan, size_t *outCount)
{
    size_t index = 0U;
    UmiUiDocumentViewSnapshot current;
    UmiStatus status = ReplacementCurrent(coordinator, plan, &index, &current);
    if (status != UMI_STATUS_OK) return status;
    if (plan->summary.text_changes) {
        UmiUiDocumentViewSnapshot after = current;
        size_t bytes = plan->summary.proposed_bytes;
        if (after.cursor_offset > bytes) after.cursor_offset = bytes;
        while (after.cursor_offset > 0U && after.cursor_offset < bytes &&
            ((unsigned char)plan->proposed[after.cursor_offset] & 0xc0U) == 0x80U)
            --after.cursor_offset;
        after.selection_length = 0U;
        after.preview = 0;
        status = CommitViewText(coordinator, index, &current, &after,
            plan->capture->text, plan->capture->length, plan->proposed, bytes);
    }
    if (status == UMI_STATUS_OK) {
        plan->consumed = 1;
        if (outCount != NULL) *outCount = plan->summary.match_count;
    }
    return status;
}
