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

typedef enum UmiEditorCompletionMatchMode {
    UMI_EDITOR_COMPLETION_MATCH_PREFIX = 1,
    UMI_EDITOR_COMPLETION_MATCH_SUBSTRING = 2,
    UMI_EDITOR_COMPLETION_MATCH_FUZZY = 3
} UmiEditorCompletionMatchMode;

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

typedef struct UmiEditorCompletionQueryResult UmiEditorCompletionQueryResult;

UmiEditorCompletionRankingConfig umi_editor_completion_ranking_config_default(
    void);
UmiStatus umi_editor_completion_query_result_create(
    UmiEditorCompletionQueryResult **out_result);
void umi_editor_completion_query_result_destroy(
    UmiEditorCompletionQueryResult *result);
UmiStatus umi_editor_completion_query_result_clear(
    UmiEditorCompletionQueryResult *result);
UmiStatus umi_editor_completion_query_execute(
    UmiEditorCompletionQueryResult *result,
    const UmiEditorCompletionCandidateCollection *candidates,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *config);
UmiStatus umi_editor_completion_query_result_at(
    const UmiEditorCompletionQueryResult *result,
    size_t position,
    UmiEditorCompletionMatch *out_match);
UmiStatus umi_editor_completion_query_result_find(
    const UmiEditorCompletionQueryResult *result,
    const char *provider_id,
    const char *candidate_id,
    UmiEditorCompletionMatch *out_match);
UmiStatus umi_editor_completion_query_result_snapshot(
    const UmiEditorCompletionQueryResult *result,
    UmiEditorCompletionQuerySnapshot *out_snapshot);
size_t umi_editor_completion_query_result_count(
    const UmiEditorCompletionQueryResult *result);
uint64_t umi_editor_completion_query_result_revision(
    const UmiEditorCompletionQueryResult *result);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_QUERY_H */
