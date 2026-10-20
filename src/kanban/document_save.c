/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/document_save.c
 * PURPOSE: Retain edits and the last saved baseline until a checked complete board write succeeds.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "document_internal.h"

static UmiStatus Refuse(UmiKanbanSaveProblem *out_problem,
    UmiKanbanSaveProblem problem, UmiStatus status)
{
    if (out_problem != NULL) *out_problem = problem;
    return status;
}

UmiStatus umi_kanban_document_save(UmiKanbanDocument *document, uint64_t expected_revision,
    UmiKanbanSaveProblem *out_problem)
{
    if (document == NULL) return Refuse(out_problem, UMI_KANBAN_SAVE_ARGUMENT,
        UMI_STATUS_INVALID_ARGUMENT);
    if (umi_kanban_board_revision(document->board) != expected_revision)
        return Refuse(out_problem, UMI_KANBAN_SAVE_STALE_REVISION, UMI_STATUS_INVALID_STATE);
    unsigned char *replacement = NULL, *observed = NULL;
    size_t replacement_size = 0U, observed_size = 0U;
    bool dirty = expected_revision != document->saved_live_revision;
    UmiStatus status = UMI_STATUS_OK;
    if (dirty) status = UmiKanbanDocumentEncode(document->board, expected_revision,
        &replacement, &replacement_size);
    if (status != UMI_STATUS_OK)
        return Refuse(out_problem, UMI_KANBAN_SAVE_ARGUMENT, status);
    /* Compare bytes rather than a timestamp or hash. Build the replacement first
     * to keep the uncooperative-writer race window as small as practical. */
    status = UmiRootedFileRead(document->directory, document->filename,
        UMI_VALUE_ARCHIVE_BYTE_LIMIT, &observed, &observed_size);
    bool changed = status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && (observed_size != document->baseline_size ||
        memcmp(observed, document->baseline, observed_size) != 0)) {
        changed = true;
        status = UMI_STATUS_INVALID_STATE;
    }
    UmiRootedFileFree(observed);
    if (status != UMI_STATUS_OK) {
        free(replacement);
        return Refuse(out_problem, changed ? UMI_KANBAN_SAVE_EXTERNAL_CHANGE :
            UMI_KANBAN_SAVE_STORAGE, status);
    }
    if (dirty) {
        status = UmiRootedFileWrite(document->directory, document->filename,
            replacement, replacement_size);
        if (status != UMI_STATUS_OK) {
            free(replacement);
            return Refuse(out_problem, UMI_KANBAN_SAVE_STORAGE, status);
        }
        free(document->baseline);
        document->baseline = replacement;
        document->baseline_size = replacement_size;
        document->saved_live_revision = expected_revision;
        document->archive_revision = expected_revision;
    }
    return Refuse(out_problem, UMI_KANBAN_SAVE_NONE, UMI_STATUS_OK);
}

UmiStatus umi_kanban_document_save_copy(const UmiKanbanDocument *document,
    uint64_t expected_revision, const char *directory, const char *filename)
{
    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_kanban_board_revision(document->board) != expected_revision) return UMI_STATUS_INVALID_STATE;
    UmiKanbanDocument *copy = NULL;
    UmiStatus status = UmiKanbanDocumentAllocate(directory, filename, &copy);
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentEncode(document->board,
        expected_revision, &copy->baseline, &copy->baseline_size);
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentAcquire(copy, true);
/* Recovery destinations use exclusive creation so a competing new file cannot be replaced. The prior call is retained for review. */
#if 0
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(copy->directory,
        copy->filename, copy->baseline, copy->baseline_size);
#endif
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentCreateBytes(copy);
    /* Recovery does not rebind, clear dirty state or adopt external content.
     * Report close errors too, while preserving any complete copy already saved. */
    UmiStatus released = UmiKanbanDocumentDispose(copy);
    return status == UMI_STATUS_OK ? released : status;
}
