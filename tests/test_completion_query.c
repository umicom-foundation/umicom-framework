/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_query.c
 *
 * PURPOSE:
 *   Verify filtering, fuzzy matching, deterministic ranking, policy handling
 *   and duplicate-edit collapse across completion providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/completion_query.h"

static void candidate_init(UmiEditorCompletionCandidate *candidate,
                           const char *provider,
                           const char *id,
                           const char *label,
                           const char *insert_text,
                           int32_t priority)
{
    (void)memset(candidate, 0, sizeof(*candidate));
    candidate->struct_size = (uint32_t)sizeof(*candidate);
    candidate->api_version = UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    candidate->item.struct_size = (uint32_t)sizeof(candidate->item);
    candidate->item.api_version = 1U;
    (void)strncpy(candidate->provider_id, provider,
                  sizeof(candidate->provider_id) - 1U);
    (void)strncpy(candidate->item.id, id, sizeof(candidate->item.id) - 1U);
    (void)strncpy(candidate->item.document_id, "document",
                  sizeof(candidate->item.document_id) - 1U);
    (void)strncpy(candidate->item.label, label,
                  sizeof(candidate->item.label) - 1U);
    (void)strncpy(candidate->item.filter_text, label,
                  sizeof(candidate->item.filter_text) - 1U);
    (void)strncpy(candidate->item.insert_text, insert_text,
                  sizeof(candidate->item.insert_text) - 1U);
    (void)strncpy(candidate->item.kind, "function",
                  sizeof(candidate->item.kind) - 1U);
    candidate->replace_start_offset = 10U;
    candidate->replace_end_offset = 12U;
    candidate->document_revision = 3U;
    candidate->request_id = 41U;
    candidate->provider_priority = priority;
    candidate->insert_format = UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    candidate->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
}

int main(void)
{
    UmiEditorCompletionCandidateCollection *collection = NULL;
    UmiEditorCompletionQueryResult *result = NULL;
    UmiEditorCompletionCandidate candidate;
    UmiEditorCompletionMatch match;
    UmiEditorCompletionQuerySnapshot snapshot;
    UmiEditorCompletionRequest request;
    UmiEditorCompletionRankingConfig config;

    assert(umi_editor_completion_candidate_collection_create(&collection) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_query_result_create(&result) == UMI_STATUS_OK);

    candidate_init(&candidate, "native", "printf", "printf", "printf", 80);
    candidate.flags = UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED;
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "lsp", "print-value", "printValue",
                   "printValue", 60);
    candidate.locality_score = 20;
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "index", "private-field", "privateField",
                   "privateField", 30);
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "ai", "ai-print", "printResult",
                   "printResult", 100);
    candidate.source = UMI_EDITOR_COMPLETION_SOURCE_AI;
    candidate.flags = UMI_EDITOR_COMPLETION_CANDIDATE_AI |
                      UMI_EDITOR_COMPLETION_CANDIDATE_REMOTE;
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "duplicate", "duplicate-printf", "printf",
                   "printf", 10);
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);

    request = umi_editor_completion_request_default("document", "c", 41U);
    (void)strncpy(request.prefix, "prf", sizeof(request.prefix) - 1U);
    request.document_revision = 3U;
    request.allow_ai = 0;
    request.allow_remote = 0;
    config = umi_editor_completion_ranking_config_default();
    assert(umi_editor_completion_query_execute(result,
                                                collection,
                                                &request,
                                                &config) == UMI_STATUS_OK);
    assert(umi_editor_completion_query_result_count(result) == 2U);
    assert(umi_editor_completion_query_result_at(result, 0U, &match) ==
           UMI_STATUS_OK);
    assert(strcmp(match.candidate.item.label, "printf") == 0);
    assert(match.matched_character_count == 3U);
    assert(umi_editor_completion_query_result_snapshot(result, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.source_candidate_count == 5U);
    assert(snapshot.filtered_candidate_count >= 2U);
    assert(snapshot.deduplicated_candidate_count == 1U);
    assert(snapshot.provider_count == 2U);
    assert(snapshot.ai_candidate_count == 0U);

    (void)strncpy(request.prefix, "printValue",
                  sizeof(request.prefix) - 1U);
    config.match_mode = UMI_EDITOR_COMPLETION_MATCH_PREFIX;
    assert(umi_editor_completion_query_execute(result,
                                                collection,
                                                &request,
                                                &config) == UMI_STATUS_OK);
    assert(umi_editor_completion_query_result_count(result) == 1U);
    assert(umi_editor_completion_query_result_at(result, 0U, &match) ==
           UMI_STATUS_OK);
    assert(match.exact_match);

    request.allow_ai = 1;
    request.allow_remote = 1;
    (void)strncpy(request.prefix, "printR", sizeof(request.prefix) - 1U);
    config.match_mode = UMI_EDITOR_COMPLETION_MATCH_FUZZY;
    config.case_sensitive = 0;
    config.smart_case = 1;
    assert(umi_editor_completion_query_execute(result,
                                                collection,
                                                &request,
                                                &config) == UMI_STATUS_OK);
    assert(umi_editor_completion_query_result_count(result) == 1U);
    assert(umi_editor_completion_query_result_at(result, 0U, &match) ==
           UMI_STATUS_OK);
    assert(strcmp(match.candidate.provider_id, "ai") == 0);

    umi_editor_completion_query_result_destroy(result);
    umi_editor_completion_candidate_collection_destroy(collection);
    return 0;
}
