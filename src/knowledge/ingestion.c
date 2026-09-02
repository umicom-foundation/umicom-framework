/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/ingestion.c
 * PURPOSE: Implement incremental source replacement across catalogue and index.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/ingestion.h"

#include <stdlib.h>
#include <string.h>

/*
 * Provide the knowledge ingest text operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_ingest_text(
    UmiKnowledgeCatalogue *catalogue,
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbeddingProvider *provider,
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        candidate.revision = current.revision + 1U;
        decision = umi_knowledge_source_refresh_decision(&current, &candidate);
        /* Apply this branch only when its contract condition is satisfied. */
        if (decision == UMI_KNOWLEDGE_REFRESH_UNCHANGED) {
            out_report->decision = decision;
            out_report->content_hash = candidate.content_hash;
            out_report->index_revision = current.revision;
            return UMI_STATUS_OK;
        }
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_NOT_FOUND) {
        decision = UMI_KNOWLEDGE_REFRESH_NEW;
        candidate.revision = source->revision != 0U ? source->revision : 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return status;
    }
    step = policy->target_bytes - policy->overlap_bytes;
    capacity = text_length / step + 2U;
    entries = (UmiKnowledgeVectorEntry *)calloc(capacity, sizeof(*entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entries == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Chunks and embeddings are interleaved in UmiKnowledgeVectorEntry, so a
     * temporary contiguous chunk array is needed for more than one record. */
    {
        UmiKnowledgeChunk *chunks = (UmiKnowledgeChunk *)calloc(
            capacity, sizeof(*chunks));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (chunks == NULL) {
            free(entries);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = umi_knowledge_chunk_text(&candidate, text, policy, chunks,
                                          capacity, &chunk_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            /* Visit each bounded item once so every record receives the same rule. */
            for (position = 0U; position < chunk_count; ++position) {
                entries[position].chunk = chunks[position];
            }
        }
        free(chunks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(entries);
        return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < chunk_count; ++position) {
        status = provider->embed_text(provider->instance,
                                      entries[position].chunk.text,
                                      &entries[position].embedding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            entries[position].embedding.dimension != provider->dimension) {
            free(entries);
            return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (decision == UMI_KNOWLEDGE_REFRESH_REPLACE) {
        status = umi_knowledge_vector_index_remove_source(
            index, candidate.source_id, &removed);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(entries);
            return status;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < chunk_count; ++position) {
        status = umi_knowledge_vector_index_upsert(
            index, &entries[position].chunk, &entries[position].embedding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(entries);
            return status;
        }
    }
    status = umi_knowledge_catalogue_upsert(catalogue, &candidate);
    free(entries);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_report->decision = decision;
    out_report->chunks_created = chunk_count;
    out_report->chunks_replaced = removed;
    out_report->content_hash = candidate.content_hash;
    out_report->index_revision = candidate.revision;
    return UMI_STATUS_OK;
}
