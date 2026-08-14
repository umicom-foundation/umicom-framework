/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/retrieval.c
 * PURPOSE: Execute vector retrieval and enrich every result with provenance.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/retrieval.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_knowledge_query_init(UmiKnowledgeQuery *query,
                                   const char *text)
{
    int written;
    if (query == NULL || text == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(query, 0, sizeof(*query));
    written = snprintf(query->text, sizeof(query->text), "%s", text);
    if (written < 0 || (size_t)written >= sizeof(query->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    query->limit = 8U;
    query->minimum_score = -1.0;
    query->filter.maximum_classification = UMI_AI_DATA_RESTRICTED;
    return UMI_STATUS_OK;
}

UmiStatus umi_knowledge_retrieve(
    const UmiKnowledgeCatalogue *catalogue,
    const UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbeddingProvider *provider,
    const UmiKnowledgeQuery *query,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count)
{
    UmiKnowledgeEmbedding embedding;
    size_t count = 0U;
    size_t read_position;
    size_t write_position = 0U;
    UmiStatus status;
    if (catalogue == NULL || index == NULL || provider == NULL ||
        query == NULL || query->text[0] == '\0' || matches == NULL ||
        capacity == 0U || out_count == NULL || query->limit == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = provider->embed_text(provider->instance, query->text, &embedding);
    if (status != UMI_STATUS_OK) return status;
    status = umi_knowledge_vector_index_search(
        index, &embedding, &query->filter, query->limit, matches, capacity,
        &count);
    if (status != UMI_STATUS_OK) return status;
    for (read_position = 0U; read_position < count; ++read_position) {
        UmiKnowledgeSource source;
        UmiKnowledgeMatch match = matches[read_position];
        if (match.score < query->minimum_score) continue;
        status = umi_knowledge_catalogue_find(
            catalogue, match.chunk.source_id, &source);
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(match.citation.source_id,
                       sizeof(match.citation.source_id), "%s",
                       source.source_id);
        (void)snprintf(match.citation.title,
                       sizeof(match.citation.title), "%s", source.title);
        (void)snprintf(match.citation.uri,
                       sizeof(match.citation.uri), "%s", source.uri);
        match.citation.byte_start = match.chunk.byte_start;
        match.citation.byte_end = match.chunk.byte_end;
        match.citation.line_start = match.chunk.line_start;
        match.citation.line_end = match.chunk.line_end;
        match.citation.source_revision = match.chunk.source_revision;
        matches[write_position++] = match;
    }
    *out_count = write_position;
    return UMI_STATUS_OK;
}
