/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/chunk.c
 *
 * PURPOSE:
 *   Create bounded text chunks with deterministic identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Deterministic chunk IDs make index rebuilds and citation references reproducible.
 */

#include "umicom/ai/chunk.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_chunk_set(UmiAiChunk *chunk,
                           const char *document_id,
                           size_t index,
                           size_t start_offset,
                           const char *text)
{
    int written;
    size_t length;
    if (chunk == NULL || document_id == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (length >= sizeof(chunk->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(chunk, 0, sizeof(*chunk));
    written = snprintf(chunk->document_id, sizeof(chunk->document_id), "%s", document_id);
    if (written < 0 || (size_t)written >= sizeof(chunk->document_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(chunk->chunk_id, sizeof(chunk->chunk_id), "%s:%zu", document_id, index);
    if (written < 0 || (size_t)written >= sizeof(chunk->chunk_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(chunk->text, text, length + 1U);
    chunk->index = index;
    chunk->start_offset = start_offset;
    chunk->end_offset = start_offset + length;
    return UMI_STATUS_OK;
}
