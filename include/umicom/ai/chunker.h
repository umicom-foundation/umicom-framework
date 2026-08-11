/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/chunker.h
 *
 * PURPOSE:
 *   Split UTF-8 text into bounded character chunks for retrieval indexing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The reference chunker uses character windows so behaviour is deterministic; smarter sentence/token chunkers can later implement the same indexing workflow.
 */

#ifndef INCLUDE_UMICOM_AI_CHUNKER_H
#define INCLUDE_UMICOM_AI_CHUNKER_H

#include <stddef.h>
#include "umicom/ai/chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_ai_chunk_text(const char *document_id,
                         const char *text,
                         size_t chunk_characters,
                         UmiAiChunk *chunks,
                         size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
