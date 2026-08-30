/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/coding_context.c
 *
 * PURPOSE:
 *   Implement deterministic repository context indexing and budget planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Active files are considered first, followed by higher priority and then path
 * order.  This deterministic ordering makes prompt context reproducible and
 * lets a review view explain every included or omitted source.
 */
#include "umicom/ai/coding_context.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingContextIndex {
    UmiAiCodingContextFile files[UMI_AI_CODING_CONTEXT_FILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_path(const UmiAiCodingContextIndex *index,
                        const char *path)
{
    size_t position;
    for (position = 0U; position < index->count; ++position) {
        if (strcmp(index->files[position].path, path) == 0) return position;
    }
    return index->count;
}

static int file_valid(const UmiAiCodingContextFile *file)
{
    return file != NULL && umi_ai_coding_path_is_safe_relative(file->path) &&
           file->estimated_tokens > 0U && file->priority > 0U &&
           file->classification >= UMI_AI_DATA_PUBLIC &&
           file->classification <= UMI_AI_DATA_RESTRICTED;
}

UmiStatus umi_ai_coding_context_index_create(
    UmiAiCodingContextIndex **out_index)
{
    UmiAiCodingContextIndex *index;
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    index = (UmiAiCodingContextIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_ai_coding_context_index_destroy(UmiAiCodingContextIndex *index)
{
    free(index);
}

UmiStatus umi_ai_coding_context_upsert(
    UmiAiCodingContextIndex *index,
    const UmiAiCodingContextFile *file)
{
    size_t position;
    if (index == NULL || !file_valid(file)) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_path(index, file->path);
    if (position == index->count) {
        if (index->count >= UMI_AI_CODING_CONTEXT_FILE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        ++index->count;
    }
    index->files[position] = *file;
    index->files[position].revision = ++index->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_context_remove(UmiAiCodingContextIndex *index,
                                       const char *path)
{
    size_t position;
    if (index == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    position = find_path(index, path);
    if (position == index->count) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < index->count) {
        (void)memmove(&index->files[position], &index->files[position + 1U],
                      (index->count - position - 1U) * sizeof(index->files[0]));
    }
    --index->count;
    ++index->revision;
    return UMI_STATUS_OK;
}

void umi_ai_coding_context_clear(UmiAiCodingContextIndex *index)
{
    if (index == NULL) return;
    (void)memset(index->files, 0, sizeof(index->files));
    index->count = 0U;
    ++index->revision;
}

size_t umi_ai_coding_context_count(const UmiAiCodingContextIndex *index)
{
    return index != NULL ? index->count : 0U;
}

UmiStatus umi_ai_coding_context_at(const UmiAiCodingContextIndex *index,
                                   size_t position,
                                   UmiAiCodingContextFile *out_file)
{
    if (index == NULL || out_file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;
    *out_file = index->files[position];
    return UMI_STATUS_OK;
}

static int preferred(const UmiAiCodingContextFile *left,
                     const UmiAiCodingContextFile *right)
{
    if (left->active != right->active) return left->active > right->active;
    if (left->priority != right->priority) return left->priority > right->priority;
    return strcmp(left->path, right->path) < 0;
}

UmiStatus umi_ai_coding_context_plan(
    const UmiAiCodingContextIndex *index,
    uint32_t token_budget,
    size_t maximum_files,
    UmiAiProviderKind provider_kind,
    const UmiAiPrivacyPolicy *privacy,
    int sensitive_approved,
    UmiAiCodingContextPlan *out_plan)
{
    size_t order[UMI_AI_CODING_CONTEXT_FILE_CAPACITY];
    size_t order_count = 0U;
    size_t left;
    size_t right;
    if (index == NULL || maximum_files == 0U ||
        maximum_files > UMI_AI_CODING_CONTEXT_PLAN_MAX || privacy == NULL ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->token_budget = token_budget;
    out_plan->remaining_tokens = token_budget;
    out_plan->highest_classification = UMI_AI_DATA_PUBLIC;
    out_plan->revision = index->revision;
    for (left = 0U; left < index->count; ++left) {
        if (index->files[left].enabled) order[order_count++] = left;
    }
    for (left = 1U; left < order_count; ++left) {
        size_t value = order[left];
        right = left;
        while (right > 0U && preferred(&index->files[value],
                                       &index->files[order[right - 1U]])) {
            order[right] = order[right - 1U];
            --right;
        }
        order[right] = value;
    }
    for (left = 0U; left < order_count; ++left) {
        const UmiAiCodingContextFile *candidate = &index->files[order[left]];
        UmiStatus share = umi_ai_privacy_policy_check_share(
            privacy, provider_kind, candidate->classification,
            sensitive_approved);
        if (share != UMI_STATUS_OK ||
            out_plan->file_count >= maximum_files ||
            candidate->estimated_tokens > out_plan->remaining_tokens) {
            ++out_plan->dropped_count;
            out_plan->truncated = 1;
            continue;
        }
        out_plan->files[out_plan->file_count++] = *candidate;
        out_plan->selected_tokens += candidate->estimated_tokens;
        out_plan->remaining_tokens -= candidate->estimated_tokens;
        if (candidate->classification > out_plan->highest_classification) {
            out_plan->highest_classification = candidate->classification;
        }
    }
    return UMI_STATUS_OK;
}
