/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/types.c
 * PURPOSE: Implement common knowledge hashes, labels and metadata filters.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/types.h"

#include <string.h>

/*
 * Provide the knowledge hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_knowledge_hash_text(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the knowledge source kind text operation used by this module and its client
 * applications.
 */
const char *umi_knowledge_source_kind_text(UmiKnowledgeSourceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_KNOWLEDGE_SOURCE_DOCUMENT: return "document";
        case UMI_KNOWLEDGE_SOURCE_CODE: return "code";
        case UMI_KNOWLEDGE_SOURCE_PROJECT: return "project";
        case UMI_KNOWLEDGE_SOURCE_WEB_ARCHIVE: return "web archive";
        case UMI_KNOWLEDGE_SOURCE_GENERATED: return "generated";
        default: return "unknown";
    }
}

/*
 * Provide the knowledge filter matches operation used by this module and its client
 * applications.
 */
int umi_knowledge_filter_matches(const UmiKnowledgeFilter *filter,
                                 const UmiKnowledgeChunk *chunk)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chunk == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->collection_id[0] != '\0' &&
        strcmp(filter->collection_id, chunk->collection_id) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->language[0] != '\0' &&
        strcmp(filter->language, chunk->language) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->media_type[0] != '\0' &&
        strcmp(filter->media_type, chunk->media_type) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->source_kind != 0 && filter->source_kind != chunk->kind) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->maximum_classification != 0 &&
        chunk->classification > filter->maximum_classification) return 0;
    return 1;
}
