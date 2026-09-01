/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/chunk.h
 *
 * PURPOSE:
 *   Represent one retrievable text chunk with source offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Chunks retain document identity and offsets so retrieval results remain traceable to the original source.
 */

#ifndef INCLUDE_UMICOM_AI_CHUNK_H
#define INCLUDE_UMICOM_AI_CHUNK_H

#include <stddef.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiChunk {
    char chunk_id[UMI_AI_ID_CAPACITY];
    char document_id[UMI_AI_ID_CAPACITY];
    size_t index;
    size_t start_offset;
    size_t end_offset;
    char text[UMI_AI_TEXT_CAPACITY];
} UmiAiChunk;

UmiStatus umi_ai_chunk_set(UmiAiChunk *chunk,
                           const char *document_id,
                           size_t index,
                           size_t start_offset,
                           const char *text);

#ifdef __cplusplus
}
#endif

#endif
