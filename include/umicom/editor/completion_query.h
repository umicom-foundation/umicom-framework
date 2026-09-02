/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_query.h
 *
 * PURPOSE:
 *   Filter, fuzzy-match, de-duplicate and deterministically rank completion
 *   candidates collected from multiple providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_QUERY_H
#define UMICOM_EDITOR_COMPLETION_QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_QUERY_API_VERSION 1U

/**
 * List the named editor completion match mode values accepted by this public contract.
 */
typedef enum UmiEditorCompletionMatchMode {
    UMI_EDITOR_COMPLETION_MATCH_PREFIX = 1,
    UMI_EDITOR_COMPLETION_MATCH_SUBSTRING = 2,
    UMI_EDITOR_COMPLETION_MATCH_FUZZY = 3
} UmiEditorCompletionMatchMode;

/**
 * Represent the editor completion ranking config data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionRankingConfig {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionMatchMode match_mode;
    size_t maximum_results;
    int32_t exact_match_bonus;
    int32_t prefix_match_bonus;
    int32_t boundary_bonus;
    int32_t consecutive_bonus;
    int32_t provider_priority_weight;
    int32_t locality_weight;
    int32_t usage_weight;
    int32_t preselected_bonus;
    int32_t deprecated_penalty;
    int32_t ai_penalty;
    int case_sensitive;
    int smart_case;
} UmiEditorCompletionRankingConfig;

/**
 * Represent the editor completion match data shared with callers of this public contract.
 */
typedef struct UmiEditorCompletionMatch {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionCandidate candidate;
    int32_t text_match_score;
    int32_t final_score;
    size_t matched_character_count;
    size_t boundary_match_count;
    size_t longest_consecutive_run;
    size_t source_position;
    int exact_match;
    int prefix_match;
} UmiEditorCompletionMatch;

/**
 * Represent the editor completion query snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t source_candidate_count;
    size_t matched_candidate_count;
    size_t filtered_candidate_count;
    size_t deduplicated_candidate_count;
    size_t provider_count;
    size_t ai_candidate_count;
    uint64_t source_revision;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t revision;
    int truncated;
    int cancelled;
} UmiEditorCompletionQuerySnapshot;

/**
 * Represent the editor completion query result data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionQueryResult UmiEditorCompletionQueryResult;

/**
 * Provide the editor completion ranking config default operation used by this module and
 * its client applications.
 */
UmiEditorCompletionRankingConfig umi_editor_completion_ranking_config_default(
    void);
/**
 * Initialise editor completion query result from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_query_result_create(
    UmiEditorCompletionQueryResult **out_result);
/**
 * Release or reset state held by editor completion query result so the same storage can be
 * reused safely.
 */
void umi_editor_completion_query_result_destroy(
    UmiEditorCompletionQueryResult *result);
/**
 * Release or reset state held by editor completion query result so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_completion_query_result_clear(
    UmiEditorCompletionQueryResult *result);
/**
 * Perform editor completion query through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_editor_completion_query_execute(
    UmiEditorCompletionQueryResult *result,
    const UmiEditorCompletionCandidateCollection *candidates,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *config);
/**
 * Find editor completion query result while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_query_result_at(
    const UmiEditorCompletionQueryResult *result,
    size_t position,
    UmiEditorCompletionMatch *out_match);
/**
 * Find editor completion query result while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_query_result_find(
    const UmiEditorCompletionQueryResult *result,
    const char *provider_id,
    const char *candidate_id,
    UmiEditorCompletionMatch *out_match);
/**
 * Provide the editor completion query result snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_query_result_snapshot(
    const UmiEditorCompletionQueryResult *result,
    UmiEditorCompletionQuerySnapshot *out_snapshot);
/**
 * Return the number of records represented by editor completion query result without
 * changing their state.
 */
size_t umi_editor_completion_query_result_count(
    const UmiEditorCompletionQueryResult *result);
/**
 * Provide the editor completion query result revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_completion_query_result_revision(
    const UmiEditorCompletionQueryResult *result);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_QUERY_H */
