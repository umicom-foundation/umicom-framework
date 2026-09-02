/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_session.c
 *
 * PURPOSE:
 *   Verify popup selection, paging, details, commit-character insertion and
 *   revision-safe application to the Framework text buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/completion_session.h"

/*
 * Initialise candidate from caller-provided values so later operations receive a known
 * state.
 */
static void candidate_init(UmiEditorCompletionCandidate *candidate,
                           const char *provider,
                           const char *id,
                           const char *label,
                           const char *text,
                           uint64_t revision,
                           uint32_t flags)
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
    (void)strncpy(candidate->item.insert_text, text,
                  sizeof(candidate->item.insert_text) - 1U);
    (void)strncpy(candidate->item.kind, "function",
                  sizeof(candidate->item.kind) - 1U);
    (void)strncpy(candidate->commit_characters, ";",
                  sizeof(candidate->commit_characters) - 1U);
    candidate->replace_start_offset = 4U;
    candidate->replace_end_offset = 7U;
    candidate->document_revision = revision;
    candidate->request_id = 55U;
    candidate->provider_priority = 10;
    candidate->insert_format = UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    candidate->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
    candidate->flags = flags;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorCompletionCandidateCollection *collection = NULL;
    UmiEditorCompletionQueryResult *result = NULL;
    UmiEditorCompletionSession *session = NULL;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorCompletionCandidate candidate;
    UmiEditorCompletionCandidate selected;
    UmiEditorCompletionAcceptance acceptance;
    UmiEditorCompletionSessionSnapshot snapshot;
    UmiEditorCompletionRequest request;
    UmiEditorCompletionRankingConfig ranking;
    UmiEditorTextBufferView view;
    uint64_t revision;

    assert(umi_editor_text_buffer_create(32U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "int pri = 0;", 12U) ==
           UMI_STATUS_OK);
    revision = umi_editor_text_buffer_revision(buffer);

    assert(umi_editor_completion_candidate_collection_create(&collection) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "native", "printf", "printf", "printf",
                   revision, 0U);
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);
    candidate_init(&candidate, "lsp", "private", "private", "private",
                   revision, UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED);
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &candidate) ==
           UMI_STATUS_OK);

    request = umi_editor_completion_request_default("document", "c", 55U);
    request.document_revision = revision;
    (void)strncpy(request.prefix, "pri", sizeof(request.prefix) - 1U);
    ranking = umi_editor_completion_ranking_config_default();
    assert(umi_editor_completion_query_result_create(&result) == UMI_STATUS_OK);
    assert(umi_editor_completion_query_execute(result,
                                                collection,
                                                &request,
                                                &ranking) == UMI_STATUS_OK);
    assert(umi_editor_completion_session_create(&session) == UMI_STATUS_OK);
    assert(umi_editor_completion_session_begin(session, result, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_session_count(session) == 2U);
    assert(umi_editor_completion_session_selected(session, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.provider_id, "lsp") == 0);
    assert(umi_editor_completion_session_select_previous(session) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_session_selected(session, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.provider_id, "native") == 0);
    assert(umi_editor_completion_session_toggle_details(session) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.details_visible);
    assert(snapshot.can_accept);

    assert(umi_editor_completion_session_apply(session,
                                                buffer,
                                                revision,
                                                ';',
                                                1,
                                                &acceptance) ==
           UMI_STATUS_OK);
    assert(strcmp(acceptance.inserted_text, "printf;") == 0);
    assert(acceptance.inserted_commit_character);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "int printf; = 0;") == 0);

    umi_editor_completion_session_destroy(session);
    session = NULL;
    assert(umi_editor_completion_session_create(&session) == UMI_STATUS_OK);
    assert(umi_editor_completion_session_begin(session, result, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_session_apply(session,
                                                buffer,
                                                revision,
                                                0,
                                                0,
                                                &acceptance) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_editor_completion_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_COMPLETION_SESSION_FAILED);

    umi_editor_completion_session_destroy(session);
    umi_editor_completion_query_result_destroy(result);
    umi_editor_completion_candidate_collection_destroy(collection);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
