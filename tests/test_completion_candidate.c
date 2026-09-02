/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_completion_candidate.c
 *
 * PURPOSE:
 *   Verify completion candidate validation, provider-qualified collection
 *   identity, normalisation, snapshots and commit-character behaviour.
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

#include "umicom/editor/completion_candidate.h"

/*
 * Initialise candidate from caller-provided values so later operations receive a known
 * state.
 */
static void candidate_init(UmiEditorCompletionCandidate *candidate,
                           const char *provider,
                           const char *id,
                           const char *label,
                           const char *insert_text)
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
    (void)strncpy(candidate->item.insert_text, insert_text,
                  sizeof(candidate->item.insert_text) - 1U);
    (void)strncpy(candidate->item.kind, "function",
                  sizeof(candidate->item.kind) - 1U);
    (void)strncpy(candidate->commit_characters, ";(",
                  sizeof(candidate->commit_characters) - 1U);
    candidate->replace_start_offset = 4U;
    candidate->replace_end_offset = 7U;
    candidate->document_revision = 11U;
    candidate->request_id = 91U;
    candidate->insert_format = UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT;
    candidate->source = UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorCompletionCandidateCollection *collection = NULL;
    UmiEditorCompletionCandidate first;
    UmiEditorCompletionCandidate second;
    UmiEditorCompletionCandidate found;
    UmiEditorCompletionCandidateCollectionSnapshot snapshot;
    uint64_t initial_revision;

    candidate_init(&first, "native", "item", "printf", "printf");
    assert(umi_editor_completion_candidate_validate(&first) == UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_accepts_commit_character(&first,
                                                                     ';'));
    assert(!umi_editor_completion_candidate_accepts_commit_character(&first,
                                                                      '.'));

    assert(umi_editor_completion_candidate_collection_create(&collection) ==
           UMI_STATUS_OK);
    initial_revision =
        umi_editor_completion_candidate_collection_revision(collection);
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_collection_count(collection) == 1U);
    assert(umi_editor_completion_candidate_collection_revision(collection) >
           initial_revision);
    assert(umi_editor_completion_candidate_collection_find(collection,
                                                             "native",
                                                             "item",
                                                             &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.item.label, "printf") == 0);

    second = first;
    (void)strncpy(second.provider_id, "lsp", sizeof(second.provider_id) - 1U);
    second.source = UMI_EDITOR_COMPLETION_SOURCE_AI;
    second.flags = UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED;
    assert(umi_editor_completion_candidate_same_edit(&first, &second));
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_collection_count(collection) == 2U);
    assert(umi_editor_completion_candidate_collection_snapshot(collection,
                                                                 &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.candidate_count == 2U);
    assert(snapshot.provider_count == 2U);
    assert(snapshot.ai_count == 1U);
    assert(snapshot.preselected_count == 1U);

    second.item.deprecated = 1;
    assert(umi_editor_completion_candidate_collection_upsert(collection,
                                                               &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_collection_snapshot(collection,
                                                                 &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.deprecated_count == 1U);

    assert(umi_editor_completion_candidate_collection_remove(collection,
                                                               "native",
                                                               "item") ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_collection_count(collection) == 1U);
    assert(umi_editor_completion_candidate_collection_clear(collection) ==
           UMI_STATUS_OK);
    assert(umi_editor_completion_candidate_collection_count(collection) == 0U);

    first.struct_size = 0U;
    assert(umi_editor_completion_candidate_validate(&first) ==
           UMI_STATUS_INVALID_ARGUMENT);
    umi_editor_completion_candidate_collection_destroy(collection);
    return 0;
}
