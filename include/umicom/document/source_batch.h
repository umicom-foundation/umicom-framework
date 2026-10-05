/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/source_batch.h
 * PURPOSE: Review and apply a complete set of captured document drafts without partial edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SOURCE_BATCH_H
#define UMICOM_DOCUMENT_SOURCE_BATCH_H
#include "umicom/document/source_request.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM 64U
#define UMI_DOCUMENT_SOURCE_BATCH_BYTE_BUDGET (64U * 1024U * 1024U)
    typedef struct UmiDocumentSourceBatch UmiDocumentSourceBatch;
    typedef struct UmiDocumentSourceBatchSummary
    {
        size_t document_count, staged_count, changed_count;
        size_t source_bytes, proposed_bytes;
        uint64_t revision;
        int applied;
    } UmiDocumentSourceBatchSummary;

    /* Capture distinct open, writable documents in caller order. Source, identity,
 * save state, conflict state and caret are retained just like a source request.
 * An invalid member rejects the entire capture. No file is opened or written.
 * Source and proposals have separate aggregate byte budgets. Use the document
 * owner's thread and retain that coordinator, store and workbench through Apply.
 * The returned batch owns its buffers; failure clears the output pointer. */
    UmiStatus UmiDocumentSourceBatchCreate(UmiDocumentCoordinator *coordinator,
                                           const UmiDocumentId *document_ids, size_t count,
                                           UmiDocumentSourceBatch **out_batch);
    void UmiDocumentSourceBatchDestroy(UmiDocumentSourceBatch *batch);
    UmiStatus UmiDocumentSourceBatchInspect(const UmiDocumentSourceBatch *batch,
                                            UmiDocumentSourceBatchSummary *out_summary);
    UmiStatus UmiDocumentSourceBatchAt(const UmiDocumentSourceBatch *batch, size_t index,
                                       UmiDocumentSourceRequestSummary *out_summary);
    /* Borrow immutable bytes until destruction; proposed bytes are also invalidated
 * by restaging that member. Source remains inspectable after Apply. */
    UmiStatus UmiDocumentSourceBatchRead(const UmiDocumentSourceBatch *batch, size_t index,
                                         const char **out_text, size_t *out_bytes);
    UmiStatus UmiDocumentSourceBatchProposed(const UmiDocumentSourceBatch *batch, size_t index,
                                             const char **out_text, size_t *out_bytes);
    /* Stage a complete replacement, including an unchanged result when appropriate.
 * Pass the BATCH revision from Inspect, not the member revision from At.
 * Success advances the batch revision and retires any previous approval.
 * Failure retains every proposal and the revision. UTF-8, zero bytes, caret
 * boundaries and aggregate budgets are checked before replacing owned storage. */
    UmiStatus UmiDocumentSourceBatchStage(UmiDocumentSourceBatch *batch, size_t index,
                                          uint64_t expected_revision, const char *text, size_t bytes,
                                          size_t cursor_offset);
    UmiStatus UmiDocumentSourceBatchCheck(UmiDocumentCoordinator *coordinator,
                                          const UmiDocumentSourceBatch *batch);
    /* Every member must be staged and the complete revision explicitly approved.
 * All source checks, view storage and Undo buffers are prepared before any text
 * changes. Failure changes no document text, history or semantic view revision.
 * Success updates the complete set, preserving active tabs and existing layout.
 * Undo remains per-document: one Undo restores that document's captured draft;
 * earlier unsynchronized typing has its own prior step. This does not introduce
 * a project-wide Undo action. Unchanged drafts retain their typing/history state.
 * No provider callbacks, filesystem writes or native event dispatch occur.
 * Success consumes the batch; destroy it even after a successful application. */
    UmiStatus UmiDocumentSourceBatchApply(UmiDocumentCoordinator *coordinator, UmiDocumentSourceBatch *batch,
                                          uint64_t reviewed_revision, int approved);
#ifdef __cplusplus
}
#endif
#endif
