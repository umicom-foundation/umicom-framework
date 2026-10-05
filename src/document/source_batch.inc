/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/source_batch.inc
 * PURPOSE: Prepare all store, view and Undo state before publishing a reviewed source edit set.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/source_batch.h"
#include "umicom/platform/document_store_edits.h"
#include "../ui/document_text_batch_internal.h"

/* Included after source_request.inc so capture and stale-state rules have one
 * implementation. The batch owns questions, not a second document model. */
struct UmiDocumentSourceBatch
{
    const UmiDocumentCoordinator *owner;
    UmiDocumentSourceRequest **requests;
    UmiDocumentSourceBatchSummary summary;
};
void UmiDocumentSourceBatchDestroy(UmiDocumentSourceBatch *batch)
{
    if (batch == NULL)
        return;
    for (size_t i = 0U; i < batch->summary.document_count; ++i)
        UmiDocumentSourceRequestDestroy(batch->requests[i]);
    free(batch->requests);
    free(batch);
}
UmiStatus UmiDocumentSourceBatchCreate(UmiDocumentCoordinator *coordinator, const UmiDocumentId *document_ids,
                                       size_t count, UmiDocumentSourceBatch **out_batch)
{
    if (out_batch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_batch = NULL;
    if (coordinator == NULL || document_ids == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i)
    {
        if (document_ids[i] == 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < i; ++prior)
            if (document_ids[prior] == document_ids[i])
                return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiDocumentSourceBatch *batch = calloc(1U, sizeof(*batch));
    if (batch == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    batch->requests = calloc(count, sizeof(*batch->requests));
    if (batch->requests == NULL)
    {
        free(batch);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    batch->owner = coordinator;
    batch->summary.document_count = count;
    batch->summary.revision = 1U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i)
    {
        status = UmiDocumentSourceRequestCreate(coordinator, document_ids[i], &batch->requests[i]);
        if (status != UMI_STATUS_OK)
            break;
        size_t bytes = batch->requests[i]->summary.source_bytes;
        if (bytes > UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET - batch->summary.source_bytes)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        batch->summary.source_bytes += bytes;
    }
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentSourceBatchDestroy(batch);
        return status;
    }
    *out_batch = batch;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceBatchInspect(const UmiDocumentSourceBatch *batch,
                                        UmiDocumentSourceBatchSummary *out_summary)
{
    if (batch == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = batch->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceBatchAt(const UmiDocumentSourceBatch *batch, size_t index,
                                   UmiDocumentSourceRequestSummary *out_summary)
{
    if (batch == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= batch->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    return UmiDocumentSourceRequestInspect(batch->requests[index], out_summary);
}
UmiStatus UmiDocumentSourceBatchRead(const UmiDocumentSourceBatch *batch, size_t index, const char **out_text,
                                     size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (batch == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= batch->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    return UmiDocumentSourceRequestRead(batch->requests[index], out_text, out_bytes);
}
UmiStatus UmiDocumentSourceBatchProposed(const UmiDocumentSourceBatch *batch, size_t index,
                                         const char **out_text, size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (batch == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= batch->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    return UmiDocumentSourceRequestProposed(batch->requests[index], out_text, out_bytes);
}
UmiStatus UmiDocumentSourceBatchStage(UmiDocumentSourceBatch *batch, size_t index, uint64_t expected_revision,
                                      const char *text, size_t bytes, size_t cursor_offset)
{
    if (batch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (batch->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (index >= batch->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    if (expected_revision != batch->summary.revision)
        return UMI_STATUS_BUSY;
    UmiDocumentSourceRequest *request = batch->requests[index];
    size_t retained_bytes = batch->summary.proposed_bytes - request->summary.proposed_bytes;
    if (batch->summary.revision == UINT64_MAX ||
        bytes > UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET - retained_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int previously_staged = request->summary.has_proposal, previously_changed = request->summary.text_changes;
    UmiStatus status =
        UmiDocumentSourceRequestStage(request, request->summary.revision, text, bytes, cursor_offset);
    if (status != UMI_STATUS_OK)
        return status;
    batch->summary.proposed_bytes = retained_bytes + bytes;
    if (!previously_staged)
        ++batch->summary.staged_count;
    if (previously_changed)
        --batch->summary.changed_count;
    if (request->summary.text_changes)
        ++batch->summary.changed_count;
    ++batch->summary.revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSourceBatchCheck(UmiDocumentCoordinator *coordinator,
                                      const UmiDocumentSourceBatch *batch)
{
    if (coordinator == NULL || batch == NULL || batch->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (batch->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < batch->summary.document_count; ++i)
    {
        UmiStatus status = UmiDocumentSourceRequestCheck(coordinator, batch->requests[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}

typedef struct SourceBatchPrepared
{
    UmiUiDocumentViewSnapshot after;
    size_t index;
    char *stored_text, *pending_text;
} SourceBatchPrepared;

/* Record visible draft changes even when they restore stored bytes, and reserve both new Undo steps within the existing history budget.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceBatchApply(UmiDocumentCoordinator *coordinator,
    UmiDocumentSourceBatch *batch, uint64_t reviewed_revision, int approved)
{
    if (coordinator == NULL || batch == NULL || batch->owner != coordinator) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (batch->summary.applied || batch->summary.staged_count != batch->summary.document_count)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != batch->summary.revision) return UMI_STATUS_BUSY;
    size_t count = batch->summary.document_count;
    SourceBatchPrepared *prepared = calloc(count, sizeof(*prepared));
    UmiUiDocumentTextBatchItem *view_changes = calloc(count, sizeof(*view_changes));
    UmiDocumentStoreTextChange *store_changes = calloc(count, sizeof(*store_changes));
    if (prepared == NULL || view_changes == NULL || store_changes == NULL) {
        free(prepared); free(view_changes); free(store_changes); return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    uint64_t view_revision = umi_ui_document_view_model_revision(views);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i) {
        UmiDocumentSourceRequest *request = batch->requests[i];
        UmiDocumentEditPlan *capture = request->target->capture;
        SourceBatchPrepared *row = &prepared[i];
        status = SourceRequestCurrent(coordinator, request, &row->index, &row->after);
        if (status != UMI_STATUS_OK) break;
        UmiDocumentSnapshot stored;
        status = UmiDocumentStoreCopySnapshot(coordinator->store, capture->stored.document_id,
            &stored, &row->stored_text);
        if (status != UMI_STATUS_OK) break;
        if (stored.revision != capture->stored.revision || stored.length != capture->stored.length) {
            status = UMI_STATUS_INVALID_STATE; break;
        }
        if (stored.length >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; break;
        }
        /* Pending typing is a separate history step. Allocate it before the
         * first store mutation, just like the single-document edit owner. */
        int pending = stored.length != capture->length || memcmp(row->stored_text, capture->text, stored.length) != 0;
        if (pending && request->summary.text_changes) {
            row->pending_text = malloc(capture->length + 1U);
            if (row->pending_text == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
            memcpy(row->pending_text, capture->text, capture->length + 1U);
        }
        row->after.cursor_offset = request->summary.proposed_cursor;
        row->after.selection_length = 0U; row->after.preview = 0;
        if (request->summary.text_changes) row->after.dirty = 1;
        view_changes[i] = (UmiUiDocumentTextBatchItem){&row->after, capture->textInfo.text_revision,
            capture->text, capture->length, request->target->proposed, request->summary.proposed_bytes};
        /* An unchanged draft must not silently commit its earlier typing to
         * the store. It still participates in identity validation. */
        store_changes[i] = (UmiDocumentStoreTextChange){&capture->stored,
            request->summary.text_changes ? request->target->proposed : row->stored_text,
            request->summary.text_changes ? request->summary.proposed_bytes : stored.length};
    }
    UmiUiDocumentTextBatch *guard = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentTextBatchAcquire(views, view_revision, view_changes, count, &guard);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentStoreReplaceTextChanges(coordinator->store, store_changes, count);
    if (status == UMI_STATUS_OK) {
        /* The store accepted the entire set. History transfer and view
         * publication allocate nothing and call no external observers. */
        for (size_t i = 0U; i < count; ++i) {
            UmiDocumentSourceRequest *request = batch->requests[i];
            UmiDocumentCoordinatorEntry *entry = &coordinator->entries[prepared[i].index];
            if (request->summary.text_changes) {
                HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].stored_text);
                prepared[i].stored_text = NULL;
                if (prepared[i].pending_text != NULL) {
                    HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].pending_text);
                    prepared[i].pending_text = NULL;
                }
                history_clear(entry->redo, &entry->redo_count); entry->pristine_virtual = 0;
            }
            request->summary.applied = 1; request->target->consumed = 1;
        }
        batch->summary.applied = 1;
        UmiUiDocumentTextBatchPublish(guard); guard = NULL;
    }
    UmiUiDocumentTextBatchAbort(guard);
    for (size_t i = 0U; i < count; ++i) { free(prepared[i].stored_text); free(prepared[i].pending_text); }
    free(prepared); free(view_changes); free(store_changes); return status;
}
#endif
/* Keep history allocations bounded independently of visible drafts and reject non-text store content before it enters Undo.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceBatchApply(UmiDocumentCoordinator *coordinator, UmiDocumentSourceBatch *batch,
                                      uint64_t reviewed_revision, int approved)
{
    if (coordinator == NULL || batch == NULL || batch->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (batch->summary.applied || batch->summary.staged_count != batch->summary.document_count)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != batch->summary.revision)
        return UMI_STATUS_BUSY;
    size_t count = batch->summary.document_count;
    SourceBatchPrepared *prepared = calloc(count, sizeof(*prepared));
    UmiUiDocumentTextBatchItem *view_changes = calloc(count, sizeof(*view_changes));
    UmiDocumentStoreTextChange *store_changes = calloc(count, sizeof(*store_changes));
    if (prepared == NULL || view_changes == NULL || store_changes == NULL)
    {
        free(prepared);
        free(view_changes);
        free(store_changes);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    uint64_t view_revision = umi_ui_document_view_model_revision(views);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentSourceRequest *request = batch->requests[i];
        UmiDocumentEditPlan *capture = request->target->capture;
        SourceBatchPrepared *row = &prepared[i];
        status = SourceRequestCurrent(coordinator, request, &row->index, &row->after);
        if (status != UMI_STATUS_OK)
            break;
        UmiDocumentSnapshot stored;
        status = UmiDocumentStoreCopySnapshot(coordinator->store, capture->stored.document_id, &stored,
                                              &row->stored_text);
        if (status != UMI_STATUS_OK)
            break;
        if (stored.revision != capture->stored.revision || stored.length != capture->stored.length)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        if (stored.length >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        /* Pending typing is a separate history step. Allocate it before the
         * first store mutation, just like the single-document edit owner. */
        int pending =
            stored.length != capture->length || memcmp(row->stored_text, capture->text, stored.length) != 0;
        if (pending && request->summary.text_changes)
        {
            if (capture->length + 1U > UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET - (stored.length + 1U))
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            row->pending_text = malloc(capture->length + 1U);
            if (row->pending_text == NULL)
            {
                status = UMI_STATUS_OUT_OF_MEMORY;
                break;
            }
            memcpy(row->pending_text, capture->text, capture->length + 1U);
        }
        row->after.cursor_offset = request->summary.proposed_cursor;
        row->after.selection_length = 0U;
        row->after.preview = 0;
        if (request->summary.text_changes)
            row->after.dirty = 1;
        view_changes[i] = (UmiUiDocumentTextBatchItem){
            &row->after,     capture->textInfo.text_revision, capture->text,
            capture->length, request->target->proposed,       request->summary.proposed_bytes};
        /* An unchanged draft must not silently commit its earlier typing to
         * the store. It still participates in identity validation. */
        store_changes[i] = (UmiDocumentStoreTextChange){
            &capture->stored, request->summary.text_changes ? request->target->proposed : row->stored_text,
            request->summary.text_changes ? request->summary.proposed_bytes : stored.length,
            request->summary.text_changes};
    }
    UmiUiDocumentTextBatch *guard = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentTextBatchAcquire(views, view_revision, view_changes, count, &guard);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentStoreReplaceTextChanges(coordinator->store, store_changes, count);
    if (status == UMI_STATUS_OK)
    {
        /* The store accepted the entire set. History transfer and view
         * publication allocate nothing and call no external observers. */
        for (size_t i = 0U; i < count; ++i)
        {
            UmiDocumentSourceRequest *request = batch->requests[i];
            UmiDocumentCoordinatorEntry *entry = &coordinator->entries[prepared[i].index];
            if (request->summary.text_changes)
            {
                HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].stored_text);
                prepared[i].stored_text = NULL;
                if (prepared[i].pending_text != NULL)
                {
                    HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].pending_text);
                    prepared[i].pending_text = NULL;
                }
                history_clear(entry->redo, &entry->redo_count);
                entry->pristine_virtual = 0;
            }
            request->summary.applied = 1;
            request->target->consumed = 1;
        }
        batch->summary.applied = 1;
        UmiUiDocumentTextBatchPublish(guard);
        guard = NULL;
    }
    UmiUiDocumentTextBatchAbort(guard);
    for (size_t i = 0U; i < count; ++i)
    {
        free(prepared[i].stored_text);
        free(prepared[i].pending_text);
    }
    free(prepared);
    free(view_changes);
    free(store_changes);
    return status;
}
#endif
/* Atomic multi-document publication now transfers captured selections with each exact prior draft; history still allocates nothing after the store accepts the complete edit set.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentSourceBatchApply(UmiDocumentCoordinator *coordinator, UmiDocumentSourceBatch *batch,
                                      uint64_t reviewed_revision, int approved)
{
    if (coordinator == NULL || batch == NULL || batch->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (batch->summary.applied || batch->summary.staged_count != batch->summary.document_count)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != batch->summary.revision)
        return UMI_STATUS_BUSY;
    size_t count = batch->summary.document_count;
    SourceBatchPrepared *prepared = calloc(count, sizeof(*prepared));
    UmiUiDocumentTextBatchItem *view_changes = calloc(count, sizeof(*view_changes));
    UmiDocumentStoreTextChange *store_changes = calloc(count, sizeof(*store_changes));
    if (prepared == NULL || view_changes == NULL || store_changes == NULL)
    {
        free(prepared);
        free(view_changes);
        free(store_changes);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    uint64_t view_revision = umi_ui_document_view_model_revision(views);
    UmiStatus status = UMI_STATUS_OK;
    size_t stored_bytes = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentSourceRequest *request = batch->requests[i];
        UmiDocumentEditPlan *capture = request->target->capture;
        SourceBatchPrepared *row = &prepared[i];
        status = SourceRequestCurrent(coordinator, request, &row->index, &row->after);
        if (status != UMI_STATUS_OK)
            break;
        /* Store history is a separate owned copy. Bound its aggregate before
         * allocation, even when a low-level caller populated a larger store. */
        if (capture->stored.length > UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES ||
            capture->stored.length > UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET - stored_bytes)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        stored_bytes += capture->stored.length;
        UmiDocumentSnapshot stored;
        status = UmiDocumentStoreCopySnapshot(coordinator->store, capture->stored.document_id, &stored,
                                              &row->stored_text);
        if (status != UMI_STATUS_OK)
            break;
        if (stored.revision != capture->stored.revision || stored.length != capture->stored.length)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        if (memchr(row->stored_text, '\0', stored.length) != NULL ||
            !umi_document_utf8_validate((const unsigned char *)row->stored_text, stored.length, NULL))
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        if (stored.length >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        /* Pending typing is a separate history step. Allocate it before the
         * first store mutation, just like the single-document edit owner. */
        int pending =
            stored.length != capture->length || memcmp(row->stored_text, capture->text, stored.length) != 0;
        if (pending && request->summary.text_changes)
        {
            if (capture->length + 1U > UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET - (stored.length + 1U))
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            row->pending_text = malloc(capture->length + 1U);
            if (row->pending_text == NULL)
            {
                status = UMI_STATUS_OUT_OF_MEMORY;
                break;
            }
            memcpy(row->pending_text, capture->text, capture->length + 1U);
        }
        row->after.cursor_offset = request->summary.proposed_cursor;
        row->after.selection_length = 0U;
        row->after.preview = 0;
        if (request->summary.text_changes)
            row->after.dirty = 1;
        view_changes[i] = (UmiUiDocumentTextBatchItem){
            &row->after,     capture->textInfo.text_revision, capture->text,
            capture->length, request->target->proposed,       request->summary.proposed_bytes};
        /* An unchanged draft must not silently commit its earlier typing to
         * the store. It still participates in identity validation. */
        store_changes[i] = (UmiDocumentStoreTextChange){
            &capture->stored, request->summary.text_changes ? request->target->proposed : row->stored_text,
            request->summary.text_changes ? request->summary.proposed_bytes : stored.length,
            request->summary.text_changes};
    }
    UmiUiDocumentTextBatch *guard = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentTextBatchAcquire(views, view_revision, view_changes, count, &guard);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentStoreReplaceTextChanges(coordinator->store, store_changes, count);
    if (status == UMI_STATUS_OK)
    {
        /* The store accepted the entire set. History transfer and view
         * publication allocate nothing and call no external observers. */
        for (size_t i = 0U; i < count; ++i)
        {
            UmiDocumentSourceRequest *request = batch->requests[i];
            UmiDocumentCoordinatorEntry *entry = &coordinator->entries[prepared[i].index];
            if (request->summary.text_changes)
            {
                HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].stored_text);
                prepared[i].stored_text = NULL;
                if (prepared[i].pending_text != NULL)
                {
                    HistoryPushOwned(entry->undo, &entry->undo_count, prepared[i].pending_text);
                    prepared[i].pending_text = NULL;
                }
                history_clear(entry->redo, &entry->redo_count);
                entry->pristine_virtual = 0;
            }
            request->summary.applied = 1;
            request->target->consumed = 1;
        }
        batch->summary.applied = 1;
        UmiUiDocumentTextBatchPublish(guard);
        guard = NULL;
    }
    UmiUiDocumentTextBatchAbort(guard);
    for (size_t i = 0U; i < count; ++i)
    {
        free(prepared[i].stored_text);
        free(prepared[i].pending_text);
    }
    free(prepared);
    free(view_changes);
    free(store_changes);
    return status;
}
#endif
UmiStatus UmiDocumentSourceBatchApply(UmiDocumentCoordinator *coordinator, UmiDocumentSourceBatch *batch,
                                      uint64_t reviewed_revision, int approved)
{
    if (coordinator == NULL || batch == NULL || batch->owner != coordinator)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (batch->summary.applied || batch->summary.staged_count != batch->summary.document_count)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != batch->summary.revision)
        return UMI_STATUS_BUSY;
    size_t count = batch->summary.document_count;
    SourceBatchPrepared *prepared = calloc(count, sizeof(*prepared));
    UmiUiDocumentTextBatchItem *view_changes = calloc(count, sizeof(*view_changes));
    UmiDocumentStoreTextChange *store_changes = calloc(count, sizeof(*store_changes));
    if (prepared == NULL || view_changes == NULL || store_changes == NULL)
    {
        free(prepared);
        free(view_changes);
        free(store_changes);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    uint64_t view_revision = umi_ui_document_view_model_revision(views);
    UmiStatus status = UMI_STATUS_OK;
    size_t stored_bytes = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentSourceRequest *request = batch->requests[i];
        UmiDocumentEditPlan *capture = request->target->capture;
        SourceBatchPrepared *row = &prepared[i];
        status = SourceRequestCurrent(coordinator, request, &row->index, &row->after);
        if (status != UMI_STATUS_OK)
            break;
        /* Store history is a separate owned copy. Bound its aggregate before
         * allocation, even when a low-level caller populated a larger store. */
        if (capture->stored.length > UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES ||
            capture->stored.length > UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET - stored_bytes)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        stored_bytes += capture->stored.length;
        UmiDocumentSnapshot stored;
        status = UmiDocumentStoreCopySnapshot(coordinator->store, capture->stored.document_id, &stored,
                                              &row->stored_text);
        if (status != UMI_STATUS_OK)
            break;
        if (stored.revision != capture->stored.revision || stored.length != capture->stored.length)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        if (memchr(row->stored_text, '\0', stored.length) != NULL ||
            !umi_document_utf8_validate((const unsigned char *)row->stored_text, stored.length, NULL))
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        if (stored.length >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        /* Pending typing is a separate history step. Allocate it before the
         * first store mutation, just like the single-document edit owner. */
        int pending =
            stored.length != capture->length || memcmp(row->stored_text, capture->text, stored.length) != 0;
        if (pending && request->summary.text_changes)
        {
            if (capture->length + 1U > UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET - (stored.length + 1U))
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            row->pending_text = malloc(capture->length + 1U);
            if (row->pending_text == NULL)
            {
                status = UMI_STATUS_OUT_OF_MEMORY;
                break;
            }
            memcpy(row->pending_text, capture->text, capture->length + 1U);
        }
        row->after.cursor_offset = request->summary.proposed_cursor;
        row->after.selection_length = 0U;
        row->after.preview = 0;
        if (request->summary.text_changes)
            row->after.dirty = 1;
        view_changes[i] = (UmiUiDocumentTextBatchItem){
            &row->after,     capture->textInfo.text_revision, capture->text,
            capture->length, request->target->proposed,       request->summary.proposed_bytes};
        /* An unchanged draft must not silently commit its earlier typing to
         * the store. It still participates in identity validation. */
        store_changes[i] = (UmiDocumentStoreTextChange){
            &capture->stored, request->summary.text_changes ? request->target->proposed : row->stored_text,
            request->summary.text_changes ? request->summary.proposed_bytes : stored.length,
            request->summary.text_changes};
    }
    UmiUiDocumentTextBatch *guard = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiUiDocumentTextBatchAcquire(views, view_revision, view_changes, count, &guard);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentStoreReplaceTextChanges(coordinator->store, store_changes, count);
    if (status == UMI_STATUS_OK)
    {
        /* The store accepted the entire set. History transfer and view
         * publication allocate nothing and call no external observers. */
        for (size_t i = 0U; i < count; ++i)
        {
            UmiDocumentSourceRequest *request = batch->requests[i];
            UmiDocumentCoordinatorEntry *entry = &coordinator->entries[prepared[i].index];
            if (request->summary.text_changes)
            {
                HistoryPushView(entry->undo, &entry->undo_count, prepared[i].stored_text,
                    prepared[i].pending_text != NULL ? NULL : &request->target->capture->view);
                prepared[i].stored_text = NULL;
                if (prepared[i].pending_text != NULL)
                {
                    HistoryPushView(entry->undo, &entry->undo_count, prepared[i].pending_text, &request->target->capture->view);
                    prepared[i].pending_text = NULL;
                }
                history_clear(entry->redo, &entry->redo_count);
                entry->pristine_virtual = 0;
            }
            request->summary.applied = 1;
            request->target->consumed = 1;
        }
        batch->summary.applied = 1;
        UmiUiDocumentTextBatchPublish(guard);
        guard = NULL;
    }
    UmiUiDocumentTextBatchAbort(guard);
    for (size_t i = 0U; i < count; ++i)
    {
        free(prepared[i].stored_text);
        free(prepared[i].pending_text);
    }
    free(prepared);
    free(view_changes);
    free(store_changes);
    return status;
}
