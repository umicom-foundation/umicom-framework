/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_request.inc
 * PURPOSE: Reuse document capture, stale-state checks and Undo for asynchronous source tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_request.h"
#include "umicom/editor/text_position.h"

/* Included after replacement_review.inc. A source request borrows no native
 * text widget and creates no second document store. The replacement capture
 * supplies save/conflict checks; this wrapper also binds the original caret. */
struct UmiDocumentSourceRequest
{
    UmiDocumentReplacementPlan *target;
    UmiDocumentSourceRequestSummary summary;
    int inspection_only;
};
void UmiDocumentSourceRequestDestroy(UmiDocumentSourceRequest *request)
{
    if (request == NULL)
        return;
    UmiDocumentReplacementPlanDestroy(request->target);
    free(request);
}
/* Source tools can inspect protected drafts through an explicitly non-editing capture.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceRequestCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                         UmiDocumentSourceRequest **out_request)
{
    if (out_request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_request = NULL;
    if (coordinator == NULL || document_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, document_id);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentSourceRequest *request = calloc(1U, sizeof(*request));
    if (request == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    request->target = calloc(1U, sizeof(*request->target));
    if (request->target == NULL)
    {
        free(request);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiDocumentReplacementPlan *target = request->target;
    UmiStatus status = UmiDocumentCoordinatorPrepareEdit(coordinator, document_id, &target->capture);
    if (status != UMI_STATUS_OK)
        goto failure;
    UmiDocumentEditPlan *capture = target->capture;
    if (capture->view.read_only)
    {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto failure;
    }
    if (capture->length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failure;
    }
    if (!umi_document_utf8_validate((const unsigned char *)capture->text, capture->length, NULL))
    {
        status = UMI_STATUS_INVALID_STATE;
        goto failure;
    }
    /* The visible view supplies the URI and language that match these bytes;
     * a saved-file snapshot alone might omit an unsaved document's identity. */
    if (strlen(capture->view.uri) >= sizeof(request->summary.uri) ||
        strlen(capture->view.language_id) >= sizeof(request->summary.language_id))
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failure;
    }
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    target->baseline = entry->baseline;
    target->encoding = entry->encoding;
    target->lineEnding = entry->line_ending;
    target->conflict = entry->conflict;
    target->summary.document_id = document_id;
    target->summary.previous_bytes = capture->length;
    request->summary.document_id = document_id;
    request->summary.source_bytes = capture->length;
    request->summary.cursor_offset = capture->view.cursor_offset;
    request->summary.selection_bytes = capture->view.selection_length;
    request->summary.revision = 1U;
    memcpy(request->summary.uri, capture->view.uri, strlen(capture->view.uri) + 1U);
    memcpy(request->summary.language_id, capture->view.language_id, strlen(capture->view.language_id) + 1U);
    (void)snprintf(request->summary.display_name, sizeof(request->summary.display_name), "%s",
                   capture->stored.display_name);
    *out_request = request;
    return UMI_STATUS_OK;
failure:
    UmiDocumentSourceRequestDestroy(request);
    return status;
}
#endif
static UmiStatus SourceRequestCapture(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                      UmiDocumentSourceRequest **out_request, int inspection_only)
{
    if (out_request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_request = NULL;
    if (coordinator == NULL || document_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, document_id);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentSourceRequest *request = calloc(1U, sizeof(*request));
    if (request == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    request->target = calloc(1U, sizeof(*request->target));
    if (request->target == NULL)
    {
        free(request);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    request->inspection_only = inspection_only;
    UmiDocumentReplacementPlan *target = request->target;
    UmiStatus status = UmiDocumentCoordinatorPrepareEdit(coordinator, document_id, &target->capture);
    if (status != UMI_STATUS_OK)
        goto failure;
    UmiDocumentEditPlan *capture = target->capture;
    if (!inspection_only && capture->view.read_only)
    {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto failure;
    }
    if (capture->length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failure;
    }
    if (!umi_document_utf8_validate((const unsigned char *)capture->text, capture->length, NULL))
    {
        status = UMI_STATUS_INVALID_STATE;
        goto failure;
    }
    /* The visible view supplies the URI and language that match these bytes;
     * a saved-file snapshot alone might omit an unsaved document's identity. */
    if (strlen(capture->view.uri) >= sizeof(request->summary.uri) ||
        strlen(capture->view.language_id) >= sizeof(request->summary.language_id))
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto failure;
    }
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    target->baseline = entry->baseline;
    target->encoding = entry->encoding;
    target->lineEnding = entry->line_ending;
    target->conflict = entry->conflict;
    target->summary.document_id = document_id;
    target->summary.previous_bytes = capture->length;
    request->summary.document_id = document_id;
    request->summary.source_bytes = capture->length;
    request->summary.cursor_offset = capture->view.cursor_offset;
    request->summary.selection_bytes = capture->view.selection_length;
    request->summary.revision = 1U;
    memcpy(request->summary.uri, capture->view.uri, strlen(capture->view.uri) + 1U);
    memcpy(request->summary.language_id, capture->view.language_id, strlen(capture->view.language_id) + 1U);
    (void)snprintf(request->summary.display_name, sizeof(request->summary.display_name), "%s",
                   capture->stored.display_name);
    *out_request = request;
    return UMI_STATUS_OK;
failure:
    UmiDocumentSourceRequestDestroy(request);
    return status;
}
UmiStatus UmiDocumentSourceRequestCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                         UmiDocumentSourceRequest **out_request)
{
    return SourceRequestCapture(coordinator, document_id, out_request, 0);
}
UmiStatus UmiDocumentSourceRequestCreateInspection(UmiDocumentCoordinator *coordinator,
                                                   UmiDocumentId document_id,
                                                   UmiDocumentSourceRequest **out_request)
{
    return SourceRequestCapture(coordinator, document_id, out_request, 1);
}

UmiStatus UmiDocumentSourceRequestInspect(const UmiDocumentSourceRequest *request,
                                          UmiDocumentSourceRequestSummary *out_summary)
{
    if (request == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = request->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceRequestRead(const UmiDocumentSourceRequest *request, const char **out_text,
                                       size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (request == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = request->target->capture->text;
    *out_bytes = request->target->capture->length;
    return UMI_STATUS_OK;
}
/* Inspection captures cannot acquire a replacement proposal even when their source is writable.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceRequestStage(UmiDocumentSourceRequest *request, uint64_t expected_revision,
                                        const char *text, size_t bytes, size_t cursor_offset)
{
    if (request == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (expected_revision != request->summary.revision)
        return UMI_STATUS_BUSY;
    if (request->summary.revision == UINT64_MAX || bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', bytes) != NULL ||
        !umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, cursor_offset, &position);
    if (status != UMI_STATUS_OK)
        return status;
    char *copy = malloc(bytes + 1U);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, text, bytes);
    copy[bytes] = '\0';
    UmiDocumentReplacementPlan *target = request->target;
    int changed = bytes != target->capture->length || memcmp(copy, target->capture->text, bytes) != 0;
    /* Finish validation and allocation before retiring the previous proposal.
     * text may itself point into that previous immutable proposal. */
    free(target->proposed);
    target->proposed = copy;
    target->summary.proposed_bytes = bytes;
    target->summary.text_changes = changed;
    request->summary.proposed_bytes = bytes;
    request->summary.proposed_cursor = cursor_offset;
    request->summary.has_proposal = 1;
    request->summary.text_changes = changed;
    ++request->summary.revision;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDocumentSourceRequestStage(UmiDocumentSourceRequest *request, uint64_t expected_revision,
                                        const char *text, size_t bytes, size_t cursor_offset)
{
    if (request == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->inspection_only)
        return UMI_STATUS_PERMISSION_DENIED;
    if (request->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (expected_revision != request->summary.revision)
        return UMI_STATUS_BUSY;
    if (request->summary.revision == UINT64_MAX || bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', bytes) != NULL ||
        !umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, cursor_offset, &position);
    if (status != UMI_STATUS_OK)
        return status;
    char *copy = malloc(bytes + 1U);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, text, bytes);
    copy[bytes] = '\0';
    UmiDocumentReplacementPlan *target = request->target;
    int changed = bytes != target->capture->length || memcmp(copy, target->capture->text, bytes) != 0;
    /* Finish validation and allocation before retiring the previous proposal.
     * text may itself point into that previous immutable proposal. */
    free(target->proposed);
    target->proposed = copy;
    target->summary.proposed_bytes = bytes;
    target->summary.text_changes = changed;
    request->summary.proposed_bytes = bytes;
    request->summary.proposed_cursor = cursor_offset;
    request->summary.has_proposal = 1;
    request->summary.text_changes = changed;
    ++request->summary.revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceRequestProposed(const UmiDocumentSourceRequest *request, const char **out_text,
                                           size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (request == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!request->summary.has_proposal || request->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    *out_text = request->target->proposed;
    *out_bytes = request->summary.proposed_bytes;
    return UMI_STATUS_OK;
}
/* Inspection reuses all source, save, path and conflict checks while retaining its captured permissions.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceRequestCurrent(UmiDocumentCoordinator *coordinator,
                                      const UmiDocumentSourceRequest *request, size_t *index,
                                      UmiUiDocumentViewSnapshot *current)
{
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = ReplacementCurrent(coordinator, request->target, index, current);
    if (status != UMI_STATUS_OK)
        return status;
    if (current->cursor_offset != request->summary.cursor_offset ||
        current->selection_length != request->summary.selection_bytes ||
        strcmp(current->language_id, request->summary.language_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus SourceRequestCurrent(UmiDocumentCoordinator *coordinator,
                                      const UmiDocumentSourceRequest *request, size_t *index,
                                      UmiUiDocumentViewSnapshot *current)
{
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status =
        ReplacementCapturedCurrent(coordinator, request->target, index, current, !request->inspection_only);
    if (status != UMI_STATUS_OK)
        return status;
    if (current->cursor_offset != request->summary.cursor_offset ||
        current->selection_length != request->summary.selection_bytes ||
        strcmp(current->language_id, request->summary.language_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceRequestCheck(UmiDocumentCoordinator *coordinator,
                                        const UmiDocumentSourceRequest *request)
{
    size_t index;
    UmiUiDocumentViewSnapshot current;
    return SourceRequestCurrent(coordinator, request, &index, &current);
}
/* A read-only request remains unable to commit edits for its entire lifetime.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceRequestApply(UmiDocumentCoordinator *coordinator,
                                        UmiDocumentSourceRequest *request, uint64_t reviewed_revision,
                                        int approved)
{
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (request->summary.applied || !request->summary.has_proposal)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != request->summary.revision)
        return UMI_STATUS_BUSY;
    size_t index;
    UmiUiDocumentViewSnapshot current;
    UmiStatus status = SourceRequestCurrent(coordinator, request, &index, &current);
    if (status != UMI_STATUS_OK)
        return status;
    UmiUiDocumentViewSnapshot after = current;
    after.cursor_offset = request->summary.proposed_cursor;
    after.selection_length = 0U;
    after.preview = 0;
    UmiDocumentReplacementPlan *target = request->target;
    /* Commit through the existing store/view/history transaction. The tool
     * never writes a widget directly and never saves a file as a side effect. */
    status = CommitViewText(coordinator, index, &current, &after, target->capture->text,
                            target->capture->length, target->proposed, request->summary.proposed_bytes);
    if (status == UMI_STATUS_OK)
    {
        request->summary.applied = 1;
        target->consumed = 1;
    }
    return status;
}
#endif
UmiStatus UmiDocumentSourceRequestApply(UmiDocumentCoordinator *coordinator,
                                        UmiDocumentSourceRequest *request, uint64_t reviewed_revision,
                                        int approved)
{
    if (request == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->inspection_only)
        return UMI_STATUS_PERMISSION_DENIED;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (request->summary.applied || !request->summary.has_proposal)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != request->summary.revision)
        return UMI_STATUS_BUSY;
    size_t index;
    UmiUiDocumentViewSnapshot current;
    UmiStatus status = SourceRequestCurrent(coordinator, request, &index, &current);
    if (status != UMI_STATUS_OK)
        return status;
    UmiUiDocumentViewSnapshot after = current;
    after.cursor_offset = request->summary.proposed_cursor;
    after.selection_length = 0U;
    after.preview = 0;
    UmiDocumentReplacementPlan *target = request->target;
    /* Commit through the existing store/view/history transaction. The tool
     * never writes a widget directly and never saves a file as a side effect. */
    status = CommitViewText(coordinator, index, &current, &after, target->capture->text,
                            target->capture->length, target->proposed, request->summary.proposed_bytes);
    if (status == UMI_STATUS_OK)
    {
        request->summary.applied = 1;
        target->consumed = 1;
    }
    return status;
}
