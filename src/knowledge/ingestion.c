/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/ingestion.c
 * PURPOSE: Implement incremental source replacement across catalogue and index.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/ingestion.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_knowledge_ingest_text(
    UmiKnowledgeCatalogue *catalogue,
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbeddingProviderV1 *provider,
    const UmiKnowledgeChunkPolicy *policy,
    const UmiKnowledgeSource *source,
    const char *text,
    UmiKnowledgeIngestionReport *out_report)
{
    UmiKnowledgeSource current;
    UmiKnowledgeSource candidate;
    UmiKnowledgeVectorEntry *entries = NULL;
    size_t text_length;
    size_t step;
    size_t capacity;
    size_t chunk_count = 0U;
    size_t removed = 0U;
    size_t position;
    UmiStatus status;
    UmiKnowledgeRefreshDecision decision;

    if (catalogue == NULL || index == NULL || provider == NULL ||
        provider->embed_text == NULL || policy == NULL || source == NULL ||
        text == NULL || text[0] == '\0' || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    candidate = *source;
    text_length = strlen(text);
    candidate.content_hash = umi_knowledge_hash_text(text);
    candidate.size_bytes = (uint64_t)text_length;
    status = umi_knowledge_catalogue_find(catalogue, source->source_id,
                                          &current);
    if (status == UMI_STATUS_OK) {
        candidate.revision = current.revision + 1U;
        decision = umi_knowledge_source_refresh_decision(&current, &candidate);
        if (decision == UMI_KNOWLEDGE_REFRESH_UNCHANGED) {
            out_report->decision = decision;
            out_report->content_hash = candidate.content_hash;
            out_report->index_revision = current.revision;
            return UMI_STATUS_OK;
        }
    } else if (status == UMI_STATUS_NOT_FOUND) {
        decision = UMI_KNOWLEDGE_REFRESH_NEW;
        candidate.revision = source->revision != 0U ? source->revision : 1U;
    } else {
        return status;
    }
    step = policy->target_bytes - policy->overlap_bytes;
    capacity = text_length / step + 2U;
    entries = (UmiKnowledgeVectorEntry *)calloc(capacity, sizeof(*entries));
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Chunks and embeddings are interleaved in UmiKnowledgeVectorEntry, so a
     * temporary contiguous chunk array is needed for more than one record. */
    {
        UmiKnowledgeChunk *chunks = (UmiKnowledgeChunk *)calloc(
            capacity, sizeof(*chunks));
        if (chunks == NULL) {
            free(entries);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = umi_knowledge_chunk_text(&candidate, text, policy, chunks,
                                          capacity, &chunk_count);
        if (status == UMI_STATUS_OK) {
            for (position = 0U; position < chunk_count; ++position) {
                entries[position].chunk = chunks[position];
            }
        }
        free(chunks);
    }
    if (status != UMI_STATUS_OK) {
        free(entries);
        return status;
    }
    for (position = 0U; position < chunk_count; ++position) {
        status = provider->embed_text(provider->instance,
                                      entries[position].chunk.text,
                                      &entries[position].embedding);
        if (status != UMI_STATUS_OK ||
            entries[position].embedding.dimension != provider->dimension) {
            free(entries);
            return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
        }
    }
    if (decision == UMI_KNOWLEDGE_REFRESH_REPLACE) {
        status = umi_knowledge_vector_index_remove_source(
            index, candidate.source_id, &removed);
        if (status != UMI_STATUS_OK) {
            free(entries);
            return status;
        }
    }
    for (position = 0U; position < chunk_count; ++position) {
        status = umi_knowledge_vector_index_upsert(
            index, &entries[position].chunk, &entries[position].embedding);
        if (status != UMI_STATUS_OK) {
            free(entries);
            return status;
        }
    }
    status = umi_knowledge_catalogue_upsert(catalogue, &candidate);
    free(entries);
    if (status != UMI_STATUS_OK) return status;
    out_report->decision = decision;
    out_report->chunks_created = chunk_count;
    out_report->chunks_replaced = removed;
    out_report->content_hash = candidate.content_hash;
    out_report->index_revision = candidate.revision;
    return UMI_STATUS_OK;
}
