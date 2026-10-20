/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/document_internal.h
 * PURPOSE: Share owned storage state between Kanban document lifecycle and save operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_DOCUMENT_INTERNAL_H
#define UMICOM_KANBAN_DOCUMENT_INTERNAL_H
#include "umicom/kanban/document.h"
#include "umicom/kanban/archive.h"
#include "umicom/base/value_archive.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/rooted_files.h"
#include <stdlib.h>
#include <string.h>
struct UmiKanbanDocument {
    char directory[UMI_PATH_CAPACITY];
    char filename[UMI_KANBAN_FILENAME_CAPACITY];
    char guard[UMI_PATH_CAPACITY];
    UmiFileLease *lease;
    UmiKanbanBoard *board;
    unsigned char *baseline;
    size_t baseline_size;
    uint64_t saved_live_revision;
    uint64_t archive_revision;
};
UmiStatus UmiKanbanDocumentAllocate(const char *directory, const char *filename,
    UmiKanbanDocument **out_document);
UmiStatus UmiKanbanDocumentAcquire(UmiKanbanDocument *document, bool create);
UmiStatus UmiKanbanDocumentEncode(const UmiKanbanBoard *board, uint64_t revision,
    unsigned char **out_bytes, size_t *out_size);
UmiStatus UmiKanbanDocumentCreateBytes(const UmiKanbanDocument *document);
UmiStatus UmiKanbanDocumentDispose(UmiKanbanDocument *document);
#endif
