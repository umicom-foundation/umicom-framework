/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/document.c
 * PURPOSE: Bind board lifetime to user-selected storage and a persistent native ownership guard.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "document_internal.h"
#include "umicom/platform/output_file.h"
#include "umicom/kanban/location.h"

UmiStatus UmiKanbanDocumentAllocate(const char *directory, const char *filename,
    UmiKanbanDocument **out_document)
{
    /* Shared location admission also serves project launchers. Preserve the former
     * inline checks so contributors can compare the unchanged archive naming rules. */
#if 0
    if (filename == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_KANBAN_FILENAME_CAPACITY && filename[length] != '\0') ++length;
    if (length == UMI_KANBAN_FILENAME_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Distinct archive/guard suffixes prevent an archive from replacing another
     * session's persistent guard. One leaf also gives all callers one convention. */
    if (length <= 6U || strcmp(filename + length - 6U, ".umikb") != 0 ||
        strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiRootedFileValidatePath(directory, filename);
    if (status != UMI_STATUS_OK) return status;
#endif
    UmiStatus status = UmiKanbanLocationValidate(directory, filename);
    if (status != UMI_STATUS_OK) return status;
    size_t length = strlen(filename);
    UmiKanbanDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    char guard_name[UMI_KANBAN_FILENAME_CAPACITY + 5U];
    memcpy(guard_name, filename, length);
    memcpy(guard_name + length, ".lock", 6U);
    status = umi_path_join(directory, guard_name, document->guard, sizeof(document->guard));
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(document->guard);
    if (status != UMI_STATUS_OK) {
        free(document);
        return status;
    }
    memcpy(document->directory, directory, strlen(directory) + 1U);
    memcpy(document->filename, filename, length + 1U);
    *out_document = document;
    return UMI_STATUS_OK;
}

UmiStatus UmiKanbanDocumentAcquire(UmiKanbanDocument *document, bool create)
{
    /* Validate the chosen directory/leaf before creating the guard. Repeat the
     * existence check under the lease so two cooperating creators cannot race. */
    for (unsigned pass = 0U; pass < 2U; ++pass) {
        int exists = 0;
        UmiStatus status = UmiRootedFileExists(document->directory, document->filename, &exists);
        if (status != UMI_STATUS_OK) return status;
        if (create && exists) return UMI_STATUS_ALREADY_EXISTS;
        if (!create && !exists) return UMI_STATUS_NOT_FOUND;
        if (pass == 0U) {
            status = UmiFileLeaseTryAcquire(document->guard, &document->lease);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiKanbanDocumentEncode(const UmiKanbanBoard *board, uint64_t revision,
    unsigned char **out_bytes, size_t *out_size)
{
    size_t size = 0U;
    UmiStatus status = umi_kanban_board_encode(board, revision, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) return status;
    unsigned char *bytes = malloc(size);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_kanban_board_encode(board, revision, bytes, size, &size);
    if (status != UMI_STATUS_OK) {
        free(bytes);
        return status;
    }
    *out_bytes = bytes;
    *out_size = size;
    return UMI_STATUS_OK;
}


/* Exclusive creation closes the check/create race even when another tool does
 * not use our guard. An I/O failure retains the partial new file as evidence;
 * never remove a path that another process may already be inspecting. */
UmiStatus UmiKanbanDocumentCreateBytes(const UmiKanbanDocument *document)
{
    char full[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(document->directory, document->filename, full, sizeof(full));
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(full, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, document->baseline, document->baseline_size);
    if (file != NULL) {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
        UmiOutputFileDestroy(file);
    }
    return status;
}

UmiStatus UmiKanbanDocumentDispose(UmiKanbanDocument *document)
{
    if (document == NULL) return UMI_STATUS_OK;
    umi_kanban_board_destroy(document->board);
    free(document->baseline);
    UmiStatus status = UmiFileLeaseRelease(&document->lease);
    free(document);
    return status;
}

UmiStatus umi_kanban_document_create(const char *directory, const char *filename,
    const UmiKanbanConfig *config, UmiKanbanDocument **out_document)
{
    if (out_document == NULL || *out_document != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanDocument *document = NULL;
    UmiStatus status = UmiKanbanDocumentAllocate(directory, filename, &document);
    if (status == UMI_STATUS_OK) status = umi_kanban_board_create(config, &document->board);
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentEncode(document->board, 1U,
        &document->baseline, &document->baseline_size);
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentAcquire(document, true);
/* New files need exclusive native creation: an existence check followed by replacement can overwrite a file created by an unrelated tool. Retain the earlier publication call for review; complete replacement remains the existing-save policy. */
#if 0
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(document->directory,
        document->filename, document->baseline, document->baseline_size);
#endif
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentCreateBytes(document);
    if (status != UMI_STATUS_OK) {
        (void)UmiKanbanDocumentDispose(document);
        return status;
    }
    /* Allocate and validate everything before publication. A successful write
     * can transfer the already prepared session without a later allocation. */
    document->saved_live_revision = 1U;
    document->archive_revision = 1U;
    *out_document = document;
    return UMI_STATUS_OK;
}

UmiStatus umi_kanban_document_open(const char *directory, const char *filename,
    UmiKanbanDocument **out_document)
{
    if (out_document == NULL || *out_document != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanDocument *document = NULL;
    UmiStatus status = UmiKanbanDocumentAllocate(directory, filename, &document);
    if (status == UMI_STATUS_OK) status = UmiKanbanDocumentAcquire(document, false);
    if (status == UMI_STATUS_OK) status = UmiRootedFileRead(document->directory,
        document->filename, UMI_VALUE_ARCHIVE_BYTE_LIMIT, &document->baseline, &document->baseline_size);
    if (status == UMI_STATUS_OK) status = umi_kanban_board_decode(document->baseline,
        document->baseline_size, &document->board, &document->archive_revision);
    if (status != UMI_STATUS_OK) {
        (void)UmiKanbanDocumentDispose(document);
        return status;
    }
    document->saved_live_revision = umi_kanban_board_revision(document->board);
    *out_document = document;
    return UMI_STATUS_OK;
}

UmiKanbanBoard *umi_kanban_document_board(UmiKanbanDocument *document)
{
    return document != NULL ? document->board : NULL;
}

UmiStatus umi_kanban_document_state(const UmiKanbanDocument *document,
    UmiKanbanDocumentState *out_state)
{
    if (document == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanDocumentState state = {0};
    memcpy(state.directory, document->directory, strlen(document->directory) + 1U);
    memcpy(state.filename, document->filename, strlen(document->filename) + 1U);
    state.board_revision = umi_kanban_board_revision(document->board);
    state.saved_live_revision = document->saved_live_revision;
    state.archive_revision = document->archive_revision;
    state.unsaved_changes = state.board_revision != state.saved_live_revision;
    *out_state = state;
    return UMI_STATUS_OK;
}

UmiStatus umi_kanban_document_close(UmiKanbanDocument **document, bool discard_changes)
{
    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*document == NULL) return UMI_STATUS_OK;
    if (!discard_changes && (*document)->saved_live_revision !=
        umi_kanban_board_revision((*document)->board)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiKanbanDocumentDispose(*document);
    *document = NULL;
    return status;
}
