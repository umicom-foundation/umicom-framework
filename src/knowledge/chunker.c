/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/chunker.c
 * PURPOSE: Create deterministic overlapping chunks with byte and line ranges.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/chunker.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the knowledge chunk policy default operation used by this module and its client
 * applications.
 */
UmiKnowledgeChunkPolicy umi_knowledge_chunk_policy_default(void)
{
    UmiKnowledgeChunkPolicy policy;
    policy.target_bytes = 1200U;
    policy.overlap_bytes = 200U;
    policy.minimum_bytes = 96U;
    policy.prefer_line_boundary = 1;
    return policy;
}

/*
 * Check that knowledge chunk policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_knowledge_chunk_policy_validate(
    const UmiKnowledgeChunkPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || policy->target_bytes == 0U ||
        policy->target_bytes >= UMI_KNOWLEDGE_TEXT_CAPACITY ||
        policy->overlap_bytes >= policy->target_bytes ||
        policy->minimum_bytes == 0U ||
        policy->minimum_bytes > policy->target_bytes) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Find line while leaving the underlying catalogue or model owned by this module. */
static size_t line_at(const char *text, size_t offset)
{
    size_t line = 1U;
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < offset; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\n') ++line;
    }
    return line;
}

/* Provide the choose end operation used by this module and its client applications. */
static size_t choose_end(const char *text, size_t length, size_t start,
                         const UmiKnowledgeChunkPolicy *policy)
{
    size_t end = start + policy->target_bytes;
    size_t lower;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (end >= length) return length;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!policy->prefer_line_boundary) return end;
    lower = start + policy->minimum_bytes;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > lower && text[end - 1U] != '\n') --end;
    return end > lower ? end : start + policy->target_bytes;
}

/*
 * Provide the knowledge chunk text operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_chunk_text(const UmiKnowledgeSource *source,
                                   const char *text,
                                   const UmiKnowledgeChunkPolicy *policy,
                                   UmiKnowledgeChunk *chunks,
                                   size_t capacity,
                                   size_t *out_count)
{
    size_t length;
    size_t start = 0U;
    size_t count = 0U;
    UmiStatus status = umi_knowledge_chunk_policy_validate(policy);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || text == NULL || chunks == NULL ||
        capacity == 0U || out_count == NULL || source->source_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_count = 0U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (start < length) {
        UmiKnowledgeChunk *chunk;
        size_t end;
        size_t amount;
        int written;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        end = choose_end(text, length, start, policy);
        amount = end - start;
        chunk = &chunks[count];
        (void)memset(chunk, 0, sizeof(*chunk));
        written = snprintf(chunk->chunk_id, sizeof(chunk->chunk_id),
                           "%s#%zu", source->source_id, count);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(chunk->chunk_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(chunk->source_id, sizeof(chunk->source_id), "%s",
                       source->source_id);
        (void)snprintf(chunk->collection_id, sizeof(chunk->collection_id),
                       "%s", source->collection_id);
        (void)snprintf(chunk->language, sizeof(chunk->language), "%s",
                       source->language);
        (void)snprintf(chunk->media_type, sizeof(chunk->media_type), "%s",
                       source->media_type);
        chunk->kind = source->kind;
        chunk->classification = source->classification;
        chunk->ordinal = count;
        chunk->byte_start = start;
        chunk->byte_end = end;
        chunk->line_start = line_at(text, start);
        chunk->line_end = line_at(text, end);
        chunk->source_revision = source->revision;
        (void)memcpy(chunk->text, text + start, amount);
        chunk->text[amount] = '\0';
        ++count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (end == length) break;
        start = end > policy->overlap_bytes
            ? end - policy->overlap_bytes : end;
    }
    *out_count = count;
    return UMI_STATUS_OK;
}
