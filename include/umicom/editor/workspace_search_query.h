/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_query.h
 *
 * PURPOSE:
 *   Define a bounded workspace-search execution and result-set contract over
 *   Framework indexed documents, compiled patterns and exclusion policies.
 *
 * THREADING:
 *   Query objects are mutable and must be confined to one worker while execute,
 *   selection or clear operations are in progress. Result values returned by
 *   at() are copies and may be safely retained by the caller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_QUERY_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/workspace_search_exclusion.h"
#include "umicom/editor/workspace_search_index.h"
#include "umicom/editor/workspace_search_match.h"
#include "umicom/editor/workspace_search_pattern.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_QUERY_API_VERSION 1U

typedef int (*UmiEditorWorkspaceSearchCancellationCheck)(void *user_data);

typedef struct UmiEditorWorkspaceSearchQueryRequest {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_results;
    size_t maximum_results_per_document;
    int allow_overlapping_matches;
    int include_empty_matches;
    int select_results_by_default;
    UmiEditorWorkspaceSearchCancellationCheck cancellation_check;
    void *cancellation_user_data;
} UmiEditorWorkspaceSearchQueryRequest;

typedef struct UmiEditorWorkspaceSearchQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t result_count;
    size_t selected_result_count;
    size_t document_count;
    size_t considered_document_count;
    size_t searched_document_count;
    size_t excluded_document_count;
    size_t binary_document_count;
    size_t read_only_result_count;
    size_t generated_result_count;
    uint64_t execution_steps;
    uint64_t source_index_revision;
    uint64_t source_pattern_revision;
    uint64_t source_exclusion_revision;
    uint64_t revision;
    int truncated;
    int cancelled;
    int complete;
} UmiEditorWorkspaceSearchQuerySnapshot;

typedef struct UmiEditorWorkspaceSearchQuery
    UmiEditorWorkspaceSearchQuery;

void umi_editor_workspace_search_query_request_init(
    UmiEditorWorkspaceSearchQueryRequest *request);

UmiStatus umi_editor_workspace_search_query_create(
    UmiEditorWorkspaceSearchQuery **out_query);
void umi_editor_workspace_search_query_destroy(
    UmiEditorWorkspaceSearchQuery *query);

UmiStatus umi_editor_workspace_search_query_execute(
    UmiEditorWorkspaceSearchQuery *query,
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchPattern *pattern,
    const UmiEditorWorkspaceSearchExclusionSet *exclusions,
    const UmiEditorWorkspaceSearchQueryRequest *request);
UmiStatus umi_editor_workspace_search_query_clear(
    UmiEditorWorkspaceSearchQuery *query);

UmiStatus umi_editor_workspace_search_query_at(
    const UmiEditorWorkspaceSearchQuery *query,
    size_t position,
    UmiEditorWorkspaceSearchMatch *out_match);
UmiStatus umi_editor_workspace_search_query_find(
    const UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    UmiEditorWorkspaceSearchMatch *out_match);
UmiStatus umi_editor_workspace_search_query_select(
    UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    int selected);
UmiStatus umi_editor_workspace_search_query_select_all(
    UmiEditorWorkspaceSearchQuery *query,
    int selected);
UmiStatus umi_editor_workspace_search_query_snapshot(
    const UmiEditorWorkspaceSearchQuery *query,
    UmiEditorWorkspaceSearchQuerySnapshot *out_snapshot);

size_t umi_editor_workspace_search_query_count(
    const UmiEditorWorkspaceSearchQuery *query);
uint64_t umi_editor_workspace_search_query_revision(
    const UmiEditorWorkspaceSearchQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_QUERY_H */
