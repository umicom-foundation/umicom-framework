/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_query.c
 *
 * PURPOSE:
 *   Execute deterministic workspace searches over a provider-populated index,
 *   applying exclusion policy before pattern evaluation and retaining bounded
 *   result values suitable for navigation and transactional replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_query.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_MAXIMUM_RESULTS 10000U
#define DEFAULT_MAXIMUM_RESULTS_PER_DOCUMENT 2000U

struct UmiEditorWorkspaceSearchQuery {
    UmiEditorWorkspaceSearchMatch *matches;
    size_t count;
    size_t capacity;
    UmiEditorWorkspaceSearchQuerySnapshot snapshot;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Check that request satisfies its contract before another service relies on it. */
static int request_valid(
    const UmiEditorWorkspaceSearchQueryRequest *request)
{
    return request != NULL &&
           request->struct_size == (uint32_t)sizeof(*request) &&
           request->api_version ==
               UMI_EDITOR_WORKSPACE_SEARCH_QUERY_API_VERSION &&
           request->maximum_results > 0U &&
           request->maximum_results_per_document > 0U;
}

/*
 * Provide the cancellation requested operation used by this module and its client
 * applications.
 */
static int cancellation_requested(
    const UmiEditorWorkspaceSearchQueryRequest *request)
{
    return request->cancellation_check != NULL &&
           request->cancellation_check(request->cancellation_user_data) != 0;
}

/* Provide the reserve matches operation used by this module and its client applications. */
static UmiStatus reserve_matches(UmiEditorWorkspaceSearchQuery *query,
                                 size_t required,
                                 size_t maximum_results)
{
    size_t capacity;
    UmiEditorWorkspaceSearchMatch *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= query->capacity) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required > maximum_results) return UMI_STATUS_CAPACITY_EXCEEDED;
    capacity = query->capacity > 0U ? query->capacity : 128U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > maximum_results) capacity = maximum_results;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity < required || capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorWorkspaceSearchMatch *)realloc(
        query->matches, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->matches = replacement;
    query->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find match operation used by this module and its client applications. */
static size_t find_match(const UmiEditorWorkspaceSearchQuery *query,
                         const char *match_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || match_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < query->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(query->matches[position].id, match_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the append match operation used by this module and its client applications. */
static UmiStatus append_match(UmiEditorWorkspaceSearchQuery *query,
                              const UmiEditorWorkspaceSearchDocumentView *document,
                              const UmiEditorWorkspaceSearchPatternMatch *found,
                              uint64_t index_revision,
                              size_t ordinal,
                              const UmiEditorWorkspaceSearchQueryRequest *request)
{
    UmiStatus status;
    status = reserve_matches(query, query->count + 1U,
                             request->maximum_results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_search_match_initialize(
        &query->matches[query->count],
        document,
        found,
        index_revision,
        ordinal);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    query->matches[query->count].selected =
        request->select_results_by_default != 0;
    ++query->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the initialise snapshot operation used by this module and its client
 * applications.
 */
static void initialise_snapshot(UmiEditorWorkspaceSearchQuery *query,
                                uint64_t index_revision,
                                uint64_t pattern_revision,
                                uint64_t exclusion_revision)
{
    (void)memset(&query->snapshot, 0, sizeof(query->snapshot));
    query->snapshot.struct_size = (uint32_t)sizeof(query->snapshot);
    query->snapshot.api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_QUERY_API_VERSION;
    query->snapshot.source_index_revision = index_revision;
    query->snapshot.source_pattern_revision = pattern_revision;
    query->snapshot.source_exclusion_revision = exclusion_revision;
}

/*
 * Initialise editor workspace search query request from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_workspace_search_query_request_init(
    UmiEditorWorkspaceSearchQueryRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_WORKSPACE_SEARCH_QUERY_API_VERSION;
    request->maximum_results = DEFAULT_MAXIMUM_RESULTS;
    request->maximum_results_per_document =
        DEFAULT_MAXIMUM_RESULTS_PER_DOCUMENT;
    request->select_results_by_default = 1;
}

/*
 * Initialise editor workspace search query from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_workspace_search_query_create(
    UmiEditorWorkspaceSearchQuery **out_query)
{
    UmiEditorWorkspaceSearchQuery *query;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_query = NULL;
    query = (UmiEditorWorkspaceSearchQuery *)calloc(1U, sizeof(*query));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->revision = 1U;
    initialise_snapshot(query, 0U, 0U, 0U);
    query->snapshot.revision = query->revision;
    *out_query = query;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor workspace search query so the same storage can be
 * reused safely.
 */
void umi_editor_workspace_search_query_destroy(
    UmiEditorWorkspaceSearchQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    free(query->matches);
    query->matches = NULL;
    free(query);
}

/*
 * Release or reset state held by editor workspace search query so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_workspace_search_query_clear(
    UmiEditorWorkspaceSearchQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    query->count = 0U;
    query->revision = next_revision(query->revision);
    initialise_snapshot(query, 0U, 0U, 0U);
    query->snapshot.revision = query->revision;
    return UMI_STATUS_OK;
}

/*
 * Perform editor workspace search query through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_editor_workspace_search_query_execute(
    UmiEditorWorkspaceSearchQuery *query,
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchPattern *pattern,
    const UmiEditorWorkspaceSearchExclusionSet *exclusions,
    const UmiEditorWorkspaceSearchQueryRequest *request)
{
    UmiEditorWorkspaceSearchPatternSnapshot pattern_snapshot;
    UmiEditorWorkspaceSearchIndexSnapshot index_snapshot;
    UmiEditorWorkspaceSearchExclusionSnapshot exclusion_snapshot;
    size_t document_position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || index == NULL || pattern == NULL ||
        exclusions == NULL || !request_valid(request)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_search_pattern_snapshot(
        pattern, &pattern_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !pattern_snapshot.compiled) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_search_index_snapshot(index, &index_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_search_exclusion_snapshot(
        exclusions, &exclusion_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    query->count = 0U;
    initialise_snapshot(query,
                        index_snapshot.revision,
                        pattern_snapshot.revision,
                        exclusion_snapshot.revision);
    /* Visit each bounded item once so every record receives the same rule. */
    for (document_position = 0U;
         document_position < index_snapshot.document_count;
         ++document_position) {
        UmiEditorWorkspaceSearchDocumentView document;
        UmiEditorWorkspaceSearchResourceFacts facts;
        UmiEditorWorkspaceSearchExclusionDecision decision;
        size_t search_offset = 0U;
        size_t ordinal = 0U;
        size_t document_results = 0U;
        int document_counted = 0;

        /* Apply this branch only when its contract condition is satisfied. */
        if (cancellation_requested(request)) {
            query->snapshot.cancelled = 1;
            break;
        }
        status = umi_editor_workspace_search_index_at(
            index, document_position, &document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        ++query->snapshot.considered_document_count;
        (void)memset(&facts, 0, sizeof(facts));
        facts.struct_size = (uint32_t)sizeof(facts);
        facts.api_version = UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION;
        facts.uri = document.uri;
        facts.relative_path = document.relative_path;
        facts.file_name = document.file_name;
        facts.extension = document.extension;
        facts.language_id = document.language_id;
        facts.hidden = document.hidden;
        facts.generated = document.generated;
        facts.binary = document.binary;
        facts.read_only = document.read_only;
        status = umi_editor_workspace_search_exclusion_evaluate(
            exclusions, &facts, &decision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!decision.included) {
            ++query->snapshot.excluded_document_count;
            /* Apply this branch only when its contract condition is satisfied. */
            if (document.binary) ++query->snapshot.binary_document_count;
            continue;
        }
        ++query->snapshot.searched_document_count;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (search_offset <= document.content_length) {
            UmiEditorWorkspaceSearchPatternMatch found;
            size_t next_offset;
            /* Apply this branch only when its contract condition is satisfied. */
            if (cancellation_requested(request)) {
                query->snapshot.cancelled = 1;
                break;
            }
            status = umi_editor_workspace_search_pattern_find(
                pattern,
                document.content,
                document.content_length,
                search_offset,
                &found);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_NOT_FOUND) break;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            query->snapshot.execution_steps += found.execution_steps;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (!found.empty || request->include_empty_matches) {
                status = append_match(query,
                                      &document,
                                      &found,
                                      index_snapshot.revision,
                                      ordinal,
                                      request);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
                    query->snapshot.truncated = 1;
                    break;
                }
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
                ++ordinal;
                ++document_results;
                document_counted = 1;
                /* Apply this branch only when its contract condition is satisfied. */
                if (document.read_only) {
                    ++query->snapshot.read_only_result_count;
                }
                /* Apply this branch only when its contract condition is satisfied. */
                if (document.generated) {
                    ++query->snapshot.generated_result_count;
                }
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (query->count >= request->maximum_results) {
                query->snapshot.truncated = 1;
                break;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (document_results >= request->maximum_results_per_document) {
                query->snapshot.truncated = 1;
                break;
            }
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (request->allow_overlapping_matches) {
                next_offset = (size_t)found.start_byte_offset + 1U;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (found.end_byte_offset > found.start_byte_offset) {
                next_offset = (size_t)found.end_byte_offset;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                next_offset = (size_t)found.start_byte_offset + 1U;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (next_offset <= search_offset) ++next_offset;
            /* Apply this branch only when its contract condition is satisfied. */
            if (next_offset > document.content_length) break;
            search_offset = next_offset;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (document_counted) ++query->snapshot.document_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (query->snapshot.cancelled || query->snapshot.truncated) break;
    }
    query->revision = next_revision(query->revision);
    query->snapshot.result_count = query->count;
    query->snapshot.selected_result_count =
        request->select_results_by_default ? query->count : 0U;
    query->snapshot.complete = !query->snapshot.cancelled &&
                               !query->snapshot.truncated;
    query->snapshot.revision = query->revision;
    return query->snapshot.cancelled ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

/*
 * Find editor workspace search query while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_workspace_search_query_at(
    const UmiEditorWorkspaceSearchQuery *query,
    size_t position,
    UmiEditorWorkspaceSearchMatch *out_match)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || out_match == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= query->count) return UMI_STATUS_NOT_FOUND;
    *out_match = query->matches[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor workspace search query while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_workspace_search_query_find(
    const UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    UmiEditorWorkspaceSearchMatch *out_match)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || match_id == NULL || match_id[0] == '\0' ||
        out_match == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_match(query, match_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_match = query->matches[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace search query select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_search_query_select(
    UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    int selected)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || match_id == NULL || match_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_match(query, match_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (query->matches[position].selected != (selected != 0)) {
        query->matches[position].selected = selected != 0;
        query->revision = next_revision(query->revision);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace search query select all operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_search_query_select_all(
    UmiEditorWorkspaceSearchQuery *query,
    int selected)
{
    size_t position;
    int changed = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < query->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (query->matches[position].selected != (selected != 0)) {
            query->matches[position].selected = selected != 0;
            changed = 1;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed) query->revision = next_revision(query->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace search query snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_search_query_snapshot(
    const UmiEditorWorkspaceSearchQuery *query,
    UmiEditorWorkspaceSearchQuerySnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = query->snapshot;
    out_snapshot->result_count = query->count;
    out_snapshot->selected_result_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < query->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (query->matches[position].selected) {
            ++out_snapshot->selected_result_count;
        }
    }
    out_snapshot->revision = query->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor workspace search query without
 * changing their state.
 */
size_t umi_editor_workspace_search_query_count(
    const UmiEditorWorkspaceSearchQuery *query)
{
    return query != NULL ? query->count : 0U;
}

/*
 * Provide the editor workspace search query revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_workspace_search_query_revision(
    const UmiEditorWorkspaceSearchQuery *query)
{
    return query != NULL ? query->revision : 0U;
}
