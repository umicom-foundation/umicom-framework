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

/*
 * Deterministic chunk IDs make index rebuilds and citation references reproducible.
 */

#include "umicom/ai/chunk.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Storage admission deliberately permits empty metadata and does not invent
 * document-offset semantics for chunks assembled directly by existing clients. */
bool umi_ai_chunk_storage_valid(const UmiAiChunk *chunk)
{
    return chunk != NULL &&
        memchr(chunk->chunk_id, '\0', sizeof(chunk->chunk_id)) != NULL &&
        memchr(chunk->document_id, '\0', sizeof(chunk->document_id)) != NULL &&
        memchr(chunk->text, '\0', sizeof(chunk->text)) != NULL;
}


/* Copy ai chunk into module-owned storage so callers keep ownership of their input values. */
/* Bounded staging preserves both borrowed strings, refuses offset overflow and publishes a complete chunk only after ID formatting succeeds; the prior constructor is The previous implementation is retained for review. */
#if 0
UmiStatus umi_ai_chunk_set(UmiAiChunk *chunk,
                           const char *document_id,
                           size_t index,
                           size_t start_offset,
                           const char *text)
{
    int written;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chunk == NULL || document_id == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(chunk->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(chunk, 0, sizeof(*chunk));
    written = snprintf(chunk->document_id, sizeof(chunk->document_id), "%s", document_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(chunk->document_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(chunk->chunk_id, sizeof(chunk->chunk_id), "%s:%zu", document_id, index);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(chunk->chunk_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(chunk->text, text, length + 1U);
    chunk->index = index;
    chunk->start_offset = start_offset;
    chunk->end_offset = start_offset + length;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_chunk_set(UmiAiChunk *chunk,
                           const char *document_id,
                           size_t index,
                           size_t start_offset,
                           const char *text)
{
    UmiAiChunk candidate = {0};
    size_t text_length = 0U;
    size_t document_length = 0U;
    int written;
    if (chunk == NULL || document_id == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Scan at most each destination extent. Input must be readable through its
     * first terminator or that extent; this does not validate arbitrary pointers. */
    while (text_length < sizeof(candidate.text) && text[text_length] != '\0')
        ++text_length;
    while (document_length < sizeof(candidate.document_id) &&
           document_id[document_length] != '\0') ++document_length;
    if (text_length == sizeof(candidate.text) ||
        document_length == sizeof(candidate.document_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (start_offset > SIZE_MAX - text_length) return UMI_STATUS_INVALID_ARGUMENT;
    /* Both borrowed strings are consumed before the live record is replaced.
     * A failed generated ID must not leave a partially constructed chunk. */
    (void)memcpy(candidate.document_id, document_id, document_length + 1U);
    (void)memcpy(candidate.text, text, text_length + 1U);
    written = snprintf(candidate.chunk_id, sizeof(candidate.chunk_id),
                       "%s:%zu", candidate.document_id, index);
    if (written < 0 || (size_t)written >= sizeof(candidate.chunk_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.index = index;
    candidate.start_offset = start_offset;
    candidate.end_offset = start_offset + text_length;
    (void)memcpy(chunk, &candidate, sizeof(candidate));
    return UMI_STATUS_OK;
}
