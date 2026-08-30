/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_candidate.c
 *
 * PURPOSE:
 *   Implement validated completion candidates and dynamically sized provider-
 *   qualified candidate collections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_candidate.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCompletionCandidateCollection {
    UmiEditorCompletionCandidate *candidates;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus reserve_candidates(
    UmiEditorCompletionCandidateCollection *collection,
    size_t required)
{
    size_t capacity;
    UmiEditorCompletionCandidate *replacement;

    if (required <= collection->capacity) return UMI_STATUS_OK;
    capacity = collection->capacity > 0U ? collection->capacity : 32U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorCompletionCandidate *)realloc(
        collection->candidates, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->candidates = replacement;
    collection->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_candidate(
    const UmiEditorCompletionCandidateCollection *collection,
    const char *provider_id,
    const char *candidate_id)
{
    size_t position;

    if (collection == NULL || provider_id == NULL || candidate_id == NULL) {
        return SIZE_MAX;
    }
    for (position = 0U; position < collection->count; ++position) {
        const UmiEditorCompletionCandidate *candidate =
            &collection->candidates[position];
        if (strcmp(candidate->provider_id, provider_id) == 0 &&
            strcmp(candidate->item.id, candidate_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_completion_candidate_validate(
    const UmiEditorCompletionCandidate *candidate)
{
    if (candidate == NULL ||
        candidate->struct_size != (uint32_t)sizeof(*candidate) ||
        candidate->api_version != UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION ||
        !terminated(candidate->provider_id, sizeof(candidate->provider_id)) ||
        candidate->provider_id[0] == '\0' ||
        !terminated(candidate->documentation,
                    sizeof(candidate->documentation)) ||
        !terminated(candidate->commit_characters,
                    sizeof(candidate->commit_characters)) ||
        !terminated(candidate->command_id, sizeof(candidate->command_id)) ||
        !terminated(candidate->item.id, sizeof(candidate->item.id)) ||
        candidate->item.id[0] == '\0' ||
        !terminated(candidate->item.document_id,
                    sizeof(candidate->item.document_id)) ||
        !terminated(candidate->item.label, sizeof(candidate->item.label)) ||
        candidate->item.label[0] == '\0' ||
        !terminated(candidate->item.detail, sizeof(candidate->item.detail)) ||
        !terminated(candidate->item.insert_text,
                    sizeof(candidate->item.insert_text)) ||
        candidate->item.insert_text[0] == '\0' ||
        !terminated(candidate->item.kind, sizeof(candidate->item.kind)) ||
        !terminated(candidate->item.sort_text,
                    sizeof(candidate->item.sort_text)) ||
        !terminated(candidate->item.filter_text,
                    sizeof(candidate->item.filter_text)) ||
        candidate->replace_end_offset < candidate->replace_start_offset ||
        candidate->insert_format < UMI_EDITOR_COMPLETION_INSERT_PLAIN_TEXT ||
        candidate->insert_format > UMI_EDITOR_COMPLETION_INSERT_SNIPPET ||
        candidate->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        candidate->source > UMI_EDITOR_COMPLETION_SOURCE_AI ||
        (candidate->flags &
         ~(UmiEditorCompletionCandidateFlags)
             UMI_EDITOR_COMPLETION_CANDIDATE_ALL_FLAGS) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiEditorCompletionCandidate normalise_candidate(
    const UmiEditorCompletionCandidate *candidate)
{
    UmiEditorCompletionCandidate stored = *candidate;

    stored.item.struct_size = (uint32_t)sizeof(stored.item);
    stored.item.api_version = 1U;
    stored.item.deprecated =
        candidate->item.deprecated != 0 ||
        (candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED) != 0U;
    if (stored.item.deprecated) {
        stored.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED;
    }
    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_AI;
    }
    return stored;
}

UmiStatus umi_editor_completion_candidate_collection_create(
    UmiEditorCompletionCandidateCollection **out_collection)
{
    UmiEditorCompletionCandidateCollection *collection;

    if (out_collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_collection = NULL;
    collection = (UmiEditorCompletionCandidateCollection *)calloc(
        1U, sizeof(*collection));
    if (collection == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->revision = 1U;
    *out_collection = collection;
    return UMI_STATUS_OK;
}

void umi_editor_completion_candidate_collection_destroy(
    UmiEditorCompletionCandidateCollection *collection)
{
    if (collection == NULL) return;
    free(collection->candidates);
    collection->candidates = NULL;
    free(collection);
}

UmiStatus umi_editor_completion_candidate_collection_clear(
    UmiEditorCompletionCandidateCollection *collection)
{
    if (collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    collection->count = 0U;
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_candidate_collection_upsert(
    UmiEditorCompletionCandidateCollection *collection,
    const UmiEditorCompletionCandidate *candidate)
{
    UmiEditorCompletionCandidate stored;
    size_t position;
    UmiStatus status;

    if (collection == NULL ||
        umi_editor_completion_candidate_validate(candidate) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_candidate(collection,
                              candidate->provider_id,
                              candidate->item.id);
    if (position == SIZE_MAX) {
        status = reserve_candidates(collection, collection->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = collection->count++;
    }
    stored = normalise_candidate(candidate);
    collection->candidates[position] = stored;
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_candidate_collection_remove(
    UmiEditorCompletionCandidateCollection *collection,
    const char *provider_id,
    const char *candidate_id)
{
    size_t position;

    if (collection == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        candidate_id == NULL || candidate_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_candidate(collection, provider_id, candidate_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (position + 1U < collection->count) {
        (void)memmove(&collection->candidates[position],
                      &collection->candidates[position + 1U],
                      (collection->count - position - 1U) *
                          sizeof(*collection->candidates));
    }
    --collection->count;
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_candidate_collection_find(
    const UmiEditorCompletionCandidateCollection *collection,
    const char *provider_id,
    const char *candidate_id,
    UmiEditorCompletionCandidate *out_candidate)
{
    size_t position;

    if (collection == NULL || provider_id == NULL || candidate_id == NULL ||
        out_candidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_candidate(collection, provider_id, candidate_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_candidate = collection->candidates[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_candidate_collection_at(
    const UmiEditorCompletionCandidateCollection *collection,
    size_t position,
    UmiEditorCompletionCandidate *out_candidate)
{
    if (collection == NULL || out_candidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= collection->count) return UMI_STATUS_NOT_FOUND;
    *out_candidate = collection->candidates[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_candidate_collection_snapshot(
    const UmiEditorCompletionCandidateCollection *collection,
    UmiEditorCompletionCandidateCollectionSnapshot *out_snapshot)
{
    size_t position;
    size_t comparison;

    if (collection == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    out_snapshot->candidate_count = collection->count;
    out_snapshot->revision = collection->revision;
    for (position = 0U; position < collection->count; ++position) {
        const UmiEditorCompletionCandidate *candidate =
            &collection->candidates[position];
        int first_provider = 1;
        for (comparison = 0U; comparison < position; ++comparison) {
            if (strcmp(candidate->provider_id,
                       collection->candidates[comparison].provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        if (first_provider) ++out_snapshot->provider_count;
        if (candidate->insert_format == UMI_EDITOR_COMPLETION_INSERT_SNIPPET) {
            ++out_snapshot->snippet_count;
        }
        if ((candidate->flags & UMI_EDITOR_COMPLETION_CANDIDATE_AI) != 0U) {
            ++out_snapshot->ai_count;
        }
        if ((candidate->flags &
             UMI_EDITOR_COMPLETION_CANDIDATE_DEPRECATED) != 0U) {
            ++out_snapshot->deprecated_count;
        }
        if ((candidate->flags &
             UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED) != 0U) {
            ++out_snapshot->preselected_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_completion_candidate_collection_count(
    const UmiEditorCompletionCandidateCollection *collection)
{
    return collection != NULL ? collection->count : 0U;
}

uint64_t umi_editor_completion_candidate_collection_revision(
    const UmiEditorCompletionCandidateCollection *collection)
{
    return collection != NULL ? collection->revision : 0U;
}

int umi_editor_completion_candidate_same_edit(
    const UmiEditorCompletionCandidate *left,
    const UmiEditorCompletionCandidate *right)
{
    if (umi_editor_completion_candidate_validate(left) != UMI_STATUS_OK ||
        umi_editor_completion_candidate_validate(right) != UMI_STATUS_OK) {
        return 0;
    }
    return strcmp(left->item.document_id, right->item.document_id) == 0 &&
           strcmp(left->item.label, right->item.label) == 0 &&
           strcmp(left->item.insert_text, right->item.insert_text) == 0 &&
           left->replace_start_offset == right->replace_start_offset &&
           left->replace_end_offset == right->replace_end_offset;
}

int umi_editor_completion_candidate_accepts_commit_character(
    const UmiEditorCompletionCandidate *candidate,
    int character)
{
    const unsigned char value = (unsigned char)character;
    const unsigned char *cursor;

    if (umi_editor_completion_candidate_validate(candidate) != UMI_STATUS_OK ||
        character <= 0 || character > 255) {
        return 0;
    }
    cursor = (const unsigned char *)candidate->commit_characters;
    while (*cursor != 0U) {
        if (*cursor == value) return 1;
        ++cursor;
    }
    return 0;
}
