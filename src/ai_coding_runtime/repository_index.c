/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/repository_index.c
 *
 * PURPOSE:
 *   Implement real repository metadata indexing into the established coding
 *   context planner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/repository_index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingRepositoryIndex {
    UmiAiCodingContextIndex *context_index;
    UmiAiCodingRepositoryIndexSnapshot snapshot;
    UmiAiDataClassification classification;
};

typedef struct RefreshContext {
    UmiAiCodingRepositoryIndex *index;
} RefreshContext;

/* Provide the estimate tokens operation used by this module and its client applications. */
static uint32_t estimate_tokens(uint64_t byte_size)
{
    const uint64_t estimate = byte_size / 4U + 1U;
    return estimate > UINT32_MAX ? UINT32_MAX : (uint32_t)estimate;
}

/* Provide the priority for operation used by this module and its client applications. */
static uint32_t priority_for(const char *path, const char *language)
{
    const char *base;
    const char *slash;

    slash = strrchr(path, '/');
    base = slash != NULL ? slash + 1 : path;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(base, "CMakeLists.txt") == 0 ||
        strcmp(base, "meson.build") == 0 ||
        strcmp(base, "Makefile") == 0) {
        return 90U;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(language, "c") == 0 ||
        strcmp(language, "cpp") == 0 ||
        strcmp(language, "rust") == 0 ||
        strcmp(language, "zig") == 0 ||
        strcmp(language, "python") == 0) {
        return 70U;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(language, "markdown") == 0) return 35U;
    return 50U;
}

/* Provide the visit entry operation used by this module and its client applications. */
static UmiStatus visit_entry(
    void *user_data,
    const UmiAiCodingScanEntry *entry,
    int *out_descend)
{
    RefreshContext *context = (RefreshContext *)user_data;
    UmiAiCodingRepositoryIndex *owner = context->index;
    UmiAiCodingContextFile file;
    const char *language;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || out_descend == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_descend = 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (entry->directory) return UMI_STATUS_OK;

    owner->snapshot.scanned_files += 1U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ai_coding_runtime_path_is_text_source(entry->relative_path)) {
        owner->snapshot.ignored_non_text_files += 1U;
        return UMI_STATUS_OK;
    }

    language = umi_ai_coding_runtime_language_for_path(entry->relative_path);

    (void)memset(&file, 0, sizeof(file));
    (void)snprintf(file.path, sizeof(file.path), "%s", entry->relative_path);
    (void)snprintf(file.language_id, sizeof(file.language_id), "%s", language);
    (void)snprintf(
        file.summary,
        sizeof(file.summary),
        "%s source file, %llu bytes",
        language,
        (unsigned long long)entry->byte_size);
    file.classification = owner->classification;
    file.estimated_tokens = estimate_tokens(entry->byte_size);
    file.priority = priority_for(entry->relative_path, language);
    file.revision = owner->snapshot.revision + 1U;
    file.active = 0;
    file.enabled = 1;

    status = umi_ai_coding_context_upsert(owner->context_index, &file);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        owner->snapshot.capacity_dropped_files += 1U;
        owner->snapshot.truncated = 1;
        return UMI_STATUS_OK;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        owner->snapshot.indexed_files += 1U;
    }

    return status;
}

/*
 * Initialise ai coding repository index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_repository_index_create(
    UmiAiCodingContextIndex *context_index,
    UmiAiCodingRepositoryIndex **out_index)
{
    UmiAiCodingRepositoryIndex *index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context_index == NULL || out_index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_index = NULL;
    index = (UmiAiCodingRepositoryIndex *)calloc(1U, sizeof(*index));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    index->context_index = context_index;
    index->snapshot.revision = 1U;
    index->classification = UMI_AI_DATA_INTERNAL;

    *out_index = index;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding repository index so the same storage can be
 * reused safely.
 */
void umi_ai_coding_repository_index_destroy(
    UmiAiCodingRepositoryIndex *index)
{
    free(index);
}

/*
 * Provide the ai coding repository index refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_repository_index_refresh(
    UmiAiCodingRepositoryIndex *index,
    const char *workspace_root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiDataClassification classification)
{
    RefreshContext context;
    size_t file_count = 0U;
    uint64_t next_revision;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || workspace_root == NULL ||
        ignore_policy == NULL ||
        classification < UMI_AI_DATA_PUBLIC ||
        classification > UMI_AI_DATA_RESTRICTED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    next_revision = index->snapshot.revision + 1U;
    (void)memset(&index->snapshot, 0, sizeof(index->snapshot));
    index->snapshot.revision = next_revision;
    index->classification = classification;

    umi_ai_coding_context_clear(index->context_index);

    context.index = index;

    status = umi_ai_coding_scan_workspace(
        workspace_root,
        ignore_policy,
        visit_entry,
        &context,
        &file_count);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * The scanner's file count includes non-text files, while scanned_files is
     * intentionally incremented only when a file reaches the indexing visitor.
     */
    if (file_count > index->snapshot.scanned_files) {
        index->snapshot.scanned_files = file_count;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding repository index snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_repository_index_snapshot(
    const UmiAiCodingRepositoryIndex *index,
    UmiAiCodingRepositoryIndexSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_snapshot = index->snapshot;
    return UMI_STATUS_OK;
}
