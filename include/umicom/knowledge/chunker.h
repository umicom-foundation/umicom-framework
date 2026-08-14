/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/chunker.h
 * PURPOSE: Split source text into overlapping, provenance-preserving chunks.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Overlap preserves context where one chunk ends and another begins. */
#ifndef UMICOM_KNOWLEDGE_CHUNKER_H
#define UMICOM_KNOWLEDGE_CHUNKER_H

#include "umicom/knowledge/source.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeChunkPolicy {
    size_t target_bytes;
    size_t overlap_bytes;
    size_t minimum_bytes;
    int prefer_line_boundary;
} UmiKnowledgeChunkPolicy;

UmiKnowledgeChunkPolicy umi_knowledge_chunk_policy_default(void);
UmiStatus umi_knowledge_chunk_policy_validate(
    const UmiKnowledgeChunkPolicy *policy);
UmiStatus umi_knowledge_chunk_text(const UmiKnowledgeSource *source,
                                   const char *text,
                                   const UmiKnowledgeChunkPolicy *policy,
                                   UmiKnowledgeChunk *chunks,
                                   size_t capacity,
                                   size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
