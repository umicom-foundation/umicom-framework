/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_candidate.h
 *
 * PURPOSE:
 *   Extend the existing completion snapshot with provider, replacement-range,
 *   documentation, commit-character, snippet and ranking metadata required by
 *   a professional completion platform.
 *
 * OWNERSHIP:
 *   Collections copy every candidate by value. Returned candidates are value
 *   snapshots and never expose internal storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_CANDIDATE_H
#define UMICOM_EDITOR_COMPLETION_CANDIDATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION 1U
#define UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY 128U
#define UMI_EDITOR_COMPLETION_DOCUMENTATION_CAPACITY 2048U
#define UMI_EDITOR_COMPLETION_COMMIT_CHARACTERS_CAPACITY 64U
#define UMI_EDITOR_COMPLETION_COMMAND_ID_CAPACITY 128U

/**
 * List the named editor completion insert format values accepted by this public contract.
 */
typedef enum UmiEditorCompletionInsertFormat {
    UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT = 1,
    UMI_EDITOR_COMPLETION_INSERT_SNIPPET = 2
} UmiEditorCompletionInsertFormat;

/**
 * List the named editor completion source values accepted by this public contract.
 */
typedef enum UmiEditorCompletionSource {
    UMI_EDITOR_COMPLETION_SOURCE_OTHER = 0,
    UMI_EDITOR_COMPLETION_SOURCE_KEYWORD = 1,
    UMI_EDITOR_COMPLETION_SOURCE_SNIPPET = 2,
    UMI_EDITOR_COMPLETION_SOURCE_DOCUMENT = 3,
    UMI_EDITOR_COMPLETION_SOURCE_WORKSPACE_INDEX = 4,
    UMI_EDITOR_COMPLETION_SOURCE_LANGUAGE_SERVER = 5,
    UMI_EDITOR_COMPLETION_SOURCE_NATIVE_ANALYSER = 6,
    UMI_EDITOR_COMPLETION_SOURCE_AI = 7
} UmiEditorCompletionSource;

typedef uint32_t UmiEditorCompletionCandidateFlags;
enum {
    UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED = 1U << 0,
    UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED = 1U << 1,
    UMI_EDITOR_COMPLETION_CANDIDATE_RESOLVED = 1U << 2,
    UMI_EDITOR_COMPLETION_CANDIDATE_INCOMPLETE = 1U << 3,
    UMI_EDITOR_COMPLETION_CANDIDATE_AI = 1U << 4,
    UMI_EDITOR_COMPLETION_CANDIDATE_REMOTE = 1U << 5,
    UMI_EDITOR_COMPLETION_CANDIDATE_REQUIRES_TRUST = 1U << 6,
    UMI_EDITOR_COMPLETION_CANDIDATE_REQUIRES_CONFIRMATION = 1U << 7,
    UMI_EDITOR_COMPLETION_CANDIDATE_ALL_FLAGS = (1U << 8) - 1U
};

/**
 * Represent the editor completion candidate data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionCandidate {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionSnapshot item;
    char provider_id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    char documentation[UMI_EDITOR_COMPLETION_DOCUMENTATION_CAPACITY];
    char commit_characters[UMI_EDITOR_COMPLETION_COMMIT_CHARACTERS_CAPACITY];
    char command_id[UMI_EDITOR_COMPLETION_COMMAND_ID_CAPACITY];
    uint64_t replace_start_offset;
    uint64_t replace_end_offset;
    uint64_t document_revision;
    uint64_t request_id;
    int32_t provider_priority;
    int32_t locality_score;
    int32_t usage_score;
    int32_t final_score;
    UmiEditorCompletionInsertFormat insert_format;
    UmiEditorCompletionSource source;
    UmiEditorCompletionCandidateFlags flags;
} UmiEditorCompletionCandidate;

/**
 * Represent the editor completion candidate collection snapshot data shared with callers
 * of this public contract.
 */
typedef struct UmiEditorCompletionCandidateCollectionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t candidate_count;
    size_t provider_count;
    size_t snippet_count;
    size_t ai_count;
    size_t deprecated_count;
    size_t preselected_count;
    uint64_t revision;
} UmiEditorCompletionCandidateCollectionSnapshot;

/**
 * Represent the editor completion candidate collection data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorCompletionCandidateCollection
    UmiEditorCompletionCandidateCollection;

/**
 * Initialise editor completion candidate collection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_candidate_collection_create(
    UmiEditorCompletionCandidateCollection **out_collection);
/**
 * Release or reset state held by editor completion candidate collection so the same
 * storage can be reused safely.
 */
void umi_editor_completion_candidate_collection_destroy(
    UmiEditorCompletionCandidateCollection *collection);
/**
 * Release or reset state held by editor completion candidate collection so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_completion_candidate_collection_clear(
    UmiEditorCompletionCandidateCollection *collection);
/**
 * Provide the editor completion candidate collection upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_completion_candidate_collection_upsert(
    UmiEditorCompletionCandidateCollection *collection,
    const UmiEditorCompletionCandidate *candidate);
/**
 * Remove editor completion candidate collection while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_editor_completion_candidate_collection_remove(
    UmiEditorCompletionCandidateCollection *collection,
    const char *provider_id,
    const char *candidate_id);
/**
 * Find editor completion candidate collection while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_candidate_collection_find(
    const UmiEditorCompletionCandidateCollection *collection,
    const char *provider_id,
    const char *candidate_id,
    UmiEditorCompletionCandidate *out_candidate);
/**
 * Find editor completion candidate collection while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_candidate_collection_at(
    const UmiEditorCompletionCandidateCollection *collection,
    size_t position,
    UmiEditorCompletionCandidate *out_candidate);
/**
 * Provide the editor completion candidate collection snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_completion_candidate_collection_snapshot(
    const UmiEditorCompletionCandidateCollection *collection,
    UmiEditorCompletionCandidateCollectionSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor completion candidate collection
 * without changing their state.
 */
size_t umi_editor_completion_candidate_collection_count(
    const UmiEditorCompletionCandidateCollection *collection);
/**
 * Provide the editor completion candidate collection revision operation used by this
 * module and its client applications.
 */
uint64_t umi_editor_completion_candidate_collection_revision(
    const UmiEditorCompletionCandidateCollection *collection);

/**
 * Check that editor completion candidate satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_editor_completion_candidate_validate(
    const UmiEditorCompletionCandidate *candidate);
/**
 * Provide the editor completion candidate same edit operation used by this module and its
 * client applications.
 */
int umi_editor_completion_candidate_same_edit(
    const UmiEditorCompletionCandidate *left,
    const UmiEditorCompletionCandidate *right);
/**
 * Provide the editor completion candidate accepts commit character operation used by this
 * module and its client applications.
 */
int umi_editor_completion_candidate_accepts_commit_character(
    const UmiEditorCompletionCandidate *candidate,
    int character);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_CANDIDATE_H */
