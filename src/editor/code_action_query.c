/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/code_action_query.c
 *
 * PURPOSE:
 *   Implement context-aware code-action aggregation with provider validation,
 *   deterministic scoring, deduplication and preferred-action ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/code_action_query.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorCodeActionQuery {
    UmiEditorRankedCodeAction *results;
    size_t count;
    size_t capacity;
    UmiEditorCodeActionQueryRequest request;
    uint64_t revision;
    int begun;
    int finalized;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int valid_trigger(UmiEditorCodeActionTrigger trigger)
{
    return trigger >= UMI_EDITOR_CODE_ACTION_TRIGGER_AUTOMATIC &&
           trigger <= UMI_EDITOR_CODE_ACTION_TRIGGER_SAVE;
}

static UmiStatus validate_request(
    const UmiEditorCodeActionQueryRequest *request)
{
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION ||
        !terminated(request->id, sizeof(request->id)) ||
        request->id[0] == '\0' ||
        !terminated(request->document_id, sizeof(request->document_id)) ||
        request->document_id[0] == '\0' ||
        !terminated(request->document_uri, sizeof(request->document_uri)) ||
        request->document_uri[0] == '\0' ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        request->language_id[0] == '\0' ||
        !terminated(request->diagnostic_id, sizeof(request->diagnostic_id)) ||
        request->requested_kind_mask == 0U ||
        (request->requested_kind_mask &
         ~(UmiEditorCodeActionKindMask)UMI_EDITOR_CODE_ACTION_KIND_ALL) != 0U ||
        !valid_trigger(request->trigger) ||
        request->end_byte_offset < request->start_byte_offset) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_candidate(const UmiEditorRankedCodeAction *candidate)
{
    if (candidate == NULL ||
        candidate->struct_size != (uint32_t)sizeof(*candidate) ||
        candidate->api_version != UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION ||
        candidate->action.struct_size !=
            (uint32_t)sizeof(candidate->action) ||
        candidate->action.api_version != 1U ||
        !terminated(candidate->action.id, sizeof(candidate->action.id)) ||
        candidate->action.id[0] == '\0' ||
        !terminated(candidate->action.document_id,
                    sizeof(candidate->action.document_id)) ||
        !terminated(candidate->action.title, sizeof(candidate->action.title)) ||
        candidate->action.title[0] == '\0' ||
        !terminated(candidate->action.kind, sizeof(candidate->action.kind)) ||
        !terminated(candidate->action.command_id,
                    sizeof(candidate->action.command_id)) ||
        !terminated(candidate->action.argument,
                    sizeof(candidate->action.argument)) ||
        !terminated(candidate->provider_id, sizeof(candidate->provider_id)) ||
        candidate->provider_id[0] == '\0' ||
        !terminated(candidate->diagnostic_id,
                    sizeof(candidate->diagnostic_id)) ||
        !terminated(candidate->disabled_reason,
                    sizeof(candidate->disabled_reason)) ||
        candidate->kind_mask == 0U ||
        (candidate->kind_mask &
         ~(UmiEditorCodeActionKindMask)UMI_EDITOR_CODE_ACTION_KIND_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_results(UmiEditorCodeActionQuery *query,
                                 size_t required)
{
    size_t capacity;
    UmiEditorRankedCodeAction *replacement;

    if (required <= query->capacity) return UMI_STATUS_OK;
    capacity = query->capacity > 0U ? query->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorRankedCodeAction *)realloc(
        query->results, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->results = replacement;
    query->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_result(const UmiEditorCodeActionQuery *query,
                          const char *action_id)
{
    size_t index;

    if (query == NULL || action_id == NULL) return SIZE_MAX;
    for (index = 0U; index < query->count; ++index) {
        if (strcmp(query->results[index].action.id, action_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int32_t ranked_score(const UmiEditorRankedCodeAction *candidate,
                            const UmiEditorCodeActionProviderItem *provider)
{
    int64_t score = (int64_t)candidate->relevance_score + provider->priority;

    if (candidate->action.preferred) score += 1000;
    if (candidate->safe) score += 100;
    if (candidate->requires_resolution) score -= 25;
    if (!candidate->action.enabled) score -= 10000;
    if (score > INT32_MAX) return INT32_MAX;
    if (score < INT32_MIN) return INT32_MIN;
    return (int32_t)score;
}

static int compare_results(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorRankedCodeAction *left =
        (const UmiEditorRankedCodeAction *)left_pointer;
    const UmiEditorRankedCodeAction *right =
        (const UmiEditorRankedCodeAction *)right_pointer;
    int order;

    if (left->action.enabled != right->action.enabled) {
        return left->action.enabled ? -1 : 1;
    }
    if (left->action.preferred != right->action.preferred) {
        return left->action.preferred ? -1 : 1;
    }
    if (left->relevance_score > right->relevance_score) return -1;
    if (left->relevance_score < right->relevance_score) return 1;
    order = strcmp(left->action.title, right->action.title);
    return order != 0 ? order : strcmp(left->action.id, right->action.id);
}

UmiStatus umi_editor_code_action_query_create(
    UmiEditorCodeActionQuery **out_query)
{
    UmiEditorCodeActionQuery *query;

    if (out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_query = NULL;
    query = (UmiEditorCodeActionQuery *)calloc(1U, sizeof(*query));
    if (query == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->revision = 1U;
    *out_query = query;
    return UMI_STATUS_OK;
}

void umi_editor_code_action_query_destroy(UmiEditorCodeActionQuery *query)
{
    if (query == NULL) return;
    free(query->results);
    query->results = NULL;
    free(query);
}

UmiStatus umi_editor_code_action_query_begin(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionQueryRequest *request)
{
    if (query == NULL || validate_request(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    query->request = *request;
    query->request.include_disabled = request->include_disabled != 0;
    query->count = 0U;
    query->begun = 1;
    query->finalized = 0;
    query->revision = next_revision(query->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_query_add(
    UmiEditorCodeActionQuery *query,
    const UmiEditorCodeActionProviderRegistry *providers,
    const UmiEditorRankedCodeAction *candidate)
{
    UmiEditorCodeActionProviderItem provider;
    UmiEditorRankedCodeAction stored;
    size_t index;
    UmiStatus status;

    if (query == NULL || providers == NULL ||
        validate_candidate(candidate) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!query->begun || query->finalized) return UMI_STATUS_INVALID_STATE;
    if ((candidate->kind_mask & query->request.requested_kind_mask) == 0U ||
        strcmp(candidate->action.document_id, query->request.document_id) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (query->request.diagnostic_id[0] != '\0' &&
        strcmp(candidate->diagnostic_id, query->request.diagnostic_id) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_editor_code_action_provider_registry_find(
        providers, candidate->provider_id, &provider);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_editor_code_action_provider_supports(
            &provider, query->request.language_id, candidate->kind_mask)) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!candidate->action.enabled && !query->request.include_disabled) {
        return UMI_STATUS_NOT_FOUND;
    }
    index = find_result(query, candidate->action.id);
    if (index == SIZE_MAX) {
        status = reserve_results(query, query->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = query->count++;
    }
    stored = *candidate;
    stored.action.preferred = candidate->action.preferred != 0;
    stored.action.enabled = candidate->action.enabled != 0;
    stored.requires_resolution = candidate->requires_resolution != 0;
    stored.safe = candidate->safe != 0;
    stored.supports_preview = candidate->supports_preview != 0 &&
                              provider.supports_preview;
    stored.provider_priority = provider.priority;
    stored.relevance_score = ranked_score(candidate, &provider);
    query->results[index] = stored;
    query->revision = next_revision(query->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_query_finalize(UmiEditorCodeActionQuery *query)
{
    if (query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!query->begun || query->finalized) return UMI_STATUS_INVALID_STATE;
    if (query->count > 1U) {
        qsort(query->results, query->count, sizeof(*query->results),
              compare_results);
    }
    query->finalized = 1;
    query->revision = next_revision(query->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_query_at(
    const UmiEditorCodeActionQuery *query,
    size_t index,
    UmiEditorRankedCodeAction *out_action)
{
    if (query == NULL || out_action == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!query->finalized) return UMI_STATUS_INVALID_STATE;
    if (index >= query->count) return UMI_STATUS_NOT_FOUND;
    *out_action = query->results[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_query_find(
    const UmiEditorCodeActionQuery *query,
    const char *action_id,
    UmiEditorRankedCodeAction *out_action)
{
    size_t index;

    if (query == NULL || action_id == NULL || out_action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!query->finalized) return UMI_STATUS_INVALID_STATE;
    index = find_result(query, action_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_action = query->results[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_code_action_query_snapshot(
    const UmiEditorCodeActionQuery *query,
    UmiEditorCodeActionQuerySnapshot *out_snapshot)
{
    size_t index;

    if (query == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION;
    out_snapshot->request = query->request;
    out_snapshot->result_count = query->count;
    out_snapshot->revision = query->revision;
    out_snapshot->finalized = query->finalized;
    for (index = 0U; index < query->count; ++index) {
        if (query->results[index].action.enabled) ++out_snapshot->enabled_count;
        if (query->results[index].action.preferred) {
            ++out_snapshot->preferred_count;
        }
        if (query->results[index].requires_resolution) {
            ++out_snapshot->unresolved_count;
        }
        if (query->results[index].safe) ++out_snapshot->safe_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_code_action_query_count(
    const UmiEditorCodeActionQuery *query)
{
    return query != NULL ? query->count : 0U;
}

uint64_t umi_editor_code_action_query_revision(
    const UmiEditorCodeActionQuery *query)
{
    return query != NULL ? query->revision : 0U;
}
