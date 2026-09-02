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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the editor workspace search query request data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor workspace search query snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor workspace search query data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWorkspaceSearchQuery
    UmiEditorWorkspaceSearchQuery;

/**
 * Initialise editor workspace search query request from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_workspace_search_query_request_init(
    UmiEditorWorkspaceSearchQueryRequest *request);

/**
 * Initialise editor workspace search query from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_workspace_search_query_create(
    UmiEditorWorkspaceSearchQuery **out_query);
/**
 * Release or reset state held by editor workspace search query so the same storage can be
 * reused safely.
 */
void umi_editor_workspace_search_query_destroy(
    UmiEditorWorkspaceSearchQuery *query);

/**
 * Perform editor workspace search query through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_editor_workspace_search_query_execute(
    UmiEditorWorkspaceSearchQuery *query,
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchPattern *pattern,
    const UmiEditorWorkspaceSearchExclusionSet *exclusions,
    const UmiEditorWorkspaceSearchQueryRequest *request);
/**
 * Release or reset state held by editor workspace search query so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_workspace_search_query_clear(
    UmiEditorWorkspaceSearchQuery *query);

/**
 * Find editor workspace search query while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_workspace_search_query_at(
    const UmiEditorWorkspaceSearchQuery *query,
    size_t position,
    UmiEditorWorkspaceSearchMatch *out_match);
/**
 * Find editor workspace search query while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_workspace_search_query_find(
    const UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    UmiEditorWorkspaceSearchMatch *out_match);
/**
 * Provide the editor workspace search query select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_search_query_select(
    UmiEditorWorkspaceSearchQuery *query,
    const char *match_id,
    int selected);
/**
 * Provide the editor workspace search query select all operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_search_query_select_all(
    UmiEditorWorkspaceSearchQuery *query,
    int selected);
/**
 * Provide the editor workspace search query snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_search_query_snapshot(
    const UmiEditorWorkspaceSearchQuery *query,
    UmiEditorWorkspaceSearchQuerySnapshot *out_snapshot);

/**
 * Return the number of records represented by editor workspace search query without
 * changing their state.
 */
size_t umi_editor_workspace_search_query_count(
    const UmiEditorWorkspaceSearchQuery *query);
/**
 * Provide the editor workspace search query revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_workspace_search_query_revision(
    const UmiEditorWorkspaceSearchQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_QUERY_H */
