/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/history_entry_internal.h
 * PURPOSE: Keep source ownership and captured caret coordinates together in bounded document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_HISTORY_ENTRY_INTERNAL_H
#define UMICOM_DOCUMENT_HISTORY_ENTRY_INTERNAL_H
#include <stddef.h>
#include "umicom/document/types.h"
/* Internal history owns text. Position metadata is optional because imported
 * or directly typed drafts may not supply the previous text's caret. Keeping
 * the metadata beside its text prevents eviction from separating the pair. */
typedef struct DocumentHistoryEntry
{
    char *text;
    size_t cursor_offset, selection_length;
    int has_position;
    /* Optional save policy travels with a format transition through Undo. */
    int has_format;
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding line_ending;
} DocumentHistoryEntry;
#endif
