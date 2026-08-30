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

UmiKnowledgeChunkPolicy umi_knowledge_chunk_policy_default(void)
{
    UmiKnowledgeChunkPolicy policy;
    policy.target_bytes = 1200U;
    policy.overlap_bytes = 200U;
    policy.minimum_bytes = 96U;
    policy.prefer_line_boundary = 1;
    return policy;
}

UmiStatus umi_knowledge_chunk_policy_validate(
    const UmiKnowledgeChunkPolicy *policy)
{
    if (policy == NULL || policy->target_bytes == 0U ||
        policy->target_bytes >= UMI_KNOWLEDGE_TEXT_CAPACITY ||
        policy->overlap_bytes >= policy->target_bytes ||
        policy->minimum_bytes == 0U ||
        policy->minimum_bytes > policy->target_bytes) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static size_t line_at(const char *text, size_t offset)
{
    size_t line = 1U;
    size_t index;
    for (index = 0U; index < offset; ++index) {
        if (text[index] == '\n') ++line;
    }
    return line;
}

static size_t choose_end(const char *text, size_t length, size_t start,
                         const UmiKnowledgeChunkPolicy *policy)
{
    size_t end = start + policy->target_bytes;
    size_t lower;
    if (end >= length) return length;
    if (!policy->prefer_line_boundary) return end;
    lower = start + policy->minimum_bytes;
    while (end > lower && text[end - 1U] != '\n') --end;
    return end > lower ? end : start + policy->target_bytes;
}

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
    if (source == NULL || text == NULL || chunks == NULL ||
        capacity == 0U || out_count == NULL || source->source_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_count = 0U;
    if (status != UMI_STATUS_OK) return status;
    length = strlen(text);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    while (start < length) {
        UmiKnowledgeChunk *chunk;
        size_t end;
        size_t amount;
        int written;
        if (count >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        end = choose_end(text, length, start, policy);
        amount = end - start;
        chunk = &chunks[count];
        (void)memset(chunk, 0, sizeof(*chunk));
        written = snprintf(chunk->chunk_id, sizeof(chunk->chunk_id),
                           "%s#%zu", source->source_id, count);
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
        if (end == length) break;
        start = end > policy->overlap_bytes
            ? end - policy->overlap_bytes : end;
    }
    *out_count = count;
    return UMI_STATUS_OK;
}
