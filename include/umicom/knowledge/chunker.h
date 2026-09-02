/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/chunker.h
 * PURPOSE: Split source text into overlapping, provenance-preserving chunks.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Overlap preserves context where one chunk ends and another begins. */
#ifndef UMICOM_KNOWLEDGE_CHUNKER_H
#define UMICOM_KNOWLEDGE_CHUNKER_H

#include "umicom/knowledge/source.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the knowledge chunk policy data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeChunkPolicy {
    size_t target_bytes;
    size_t overlap_bytes;
    size_t minimum_bytes;
    int prefer_line_boundary;
} UmiKnowledgeChunkPolicy;

/**
 * Provide the knowledge chunk policy default operation used by this module and its client
 * applications.
 */
UmiKnowledgeChunkPolicy umi_knowledge_chunk_policy_default(void);
/**
 * Check that knowledge chunk policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_knowledge_chunk_policy_validate(
    const UmiKnowledgeChunkPolicy *policy);
/**
 * Provide the knowledge chunk text operation used by this module and its client
 * applications.
 */
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
