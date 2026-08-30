/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/chunker.c
 *
 * PURPOSE:
 *   Split bounded text into deterministic retrieval chunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This simple implementation creates stable testable chunks and makes capacity limits visible to the caller.
 */

#include "umicom/ai/chunker.h"
#include <stddef.h>

#include <string.h>

size_t umi_ai_chunk_text(const char *document_id,
                         const char *text,
                         size_t chunk_characters,
                         UmiAiChunk *chunks,
                         size_t capacity)
{
    size_t length;
    size_t offset = 0U;
    size_t count = 0U;
    if (document_id == NULL || text == NULL || chunks == NULL ||
        chunk_characters == 0U || capacity == 0U) {
        return 0U;
    }
    if (chunk_characters >= UMI_AI_TEXT_CAPACITY) {
        chunk_characters = UMI_AI_TEXT_CAPACITY - 1U;
    }
    length = strlen(text);
    while (offset < length && count < capacity) {
        size_t take = length - offset;
        char buffer[UMI_AI_TEXT_CAPACITY];
        if (take > chunk_characters) take = chunk_characters;
        (void)memcpy(buffer, text + offset, take);
        buffer[take] = '\0';
        if (umi_ai_chunk_set(&chunks[count], document_id, count, offset, buffer) != UMI_STATUS_OK) {
            break;
        }
        ++count;
        offset += take;
    }
    return count;
}
