/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/checkpoint.c
 *
 * PURPOSE:
 *   Implement complete-file checkpoint capture/restore with conflict-free
 *   bounded storage owned by the Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/checkpoint.h"

#include <stdio.h>
#include <string.h>

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                store->checkpoints[index].checkpoint_id,
                checkpoint_id) == 0) {
            return index;
        }
    }

    return store->count;
}

/*
 * Initialise ai coding checkpoint store from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_checkpoint_store_init(UmiAiCodingCheckpointStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) (void)memset(store, 0, sizeof(*store));
}

/*
 * Provide the ai coding checkpoint capture operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_checkpoint_capture(
    UmiAiCodingCheckpointStore *store,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *checkpoint_id,
    const char *label,
    const char *const *paths,
    size_t path_count)
{
    UmiAiCodingCheckpoint checkpoint;
    size_t index;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || workspace == NULL ||
        checkpoint_id == NULL || checkpoint_id[0] == '\0' ||
        label == NULL || paths == NULL || path_count == 0U ||
        path_count > UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&checkpoint, 0, sizeof(checkpoint));

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(checkpoint_id) >= sizeof(checkpoint.checkpoint_id) ||
        strlen(label) >= sizeof(checkpoint.label)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(checkpoint.checkpoint_id, checkpoint_id);
    (void)strcpy(checkpoint.label, label);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < path_count; ++index) {
        UmiAiCodingCheckpointFile *file =
            &checkpoint.files[checkpoint.file_count];
        int exists = 0;
        size_t length = 0U;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (paths[index] == NULL ||
            strlen(paths[index]) >= sizeof(file->path)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        status = workspace->exists(
            workspace->user_data,
            paths[index],
            &exists);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        (void)strcpy(file->path, paths[index]);
        file->existed = exists;

        /* Use the optional file only when it is present in this checkout. */
        if (exists) {
            status = workspace->read(
                workspace->user_data,
                paths[index],
                file->text,
                sizeof(file->text),
                &length);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            file->length = length;
            file->hash = umi_ai_coding_text_hash(file->text, length);
        }

        checkpoint.file_count += 1U;
    }

    checkpoint.revision = ++store->sequence;

    position = find_index(store, checkpoint_id);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position == store->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->count == UMI_AI_CODING_TOOL_CHECKPOINT_CAPACITY) {
            (void)memmove(
                &store->checkpoints[0],
                &store->checkpoints[1],
                (store->count - 1U) * sizeof(store->checkpoints[0]));
            store->count -= 1U;
            position = store->count;
        }
        store->count += 1U;
    }

    store->checkpoints[position] = checkpoint;
    return UMI_STATUS_OK;
}

/*
 * Find ai coding checkpoint while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_checkpoint_find(
    const UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id,
    UmiAiCodingCheckpoint *out_checkpoint)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || checkpoint_id == NULL ||
        out_checkpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(store, checkpoint_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position == store->count) return UMI_STATUS_NOT_FOUND;

    *out_checkpoint = store->checkpoints[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding checkpoint restore operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_checkpoint_restore(
    UmiAiCodingCheckpointStore *store,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *checkpoint_id)
{
    UmiAiCodingCheckpoint checkpoint;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || workspace == NULL ||
        checkpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_checkpoint_find(
        store, checkpoint_id, &checkpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < checkpoint.file_count; ++index) {
        const UmiAiCodingCheckpointFile *file =
            &checkpoint.files[index];
        int exists = 0;

        status = workspace->exists(
            workspace->user_data,
            file->path,
            &exists);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (file->existed) {
            status = workspace->write(
                workspace->user_data,
                file->path,
                file->text,
                file->length);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (exists) {
            status = workspace->remove(
                workspace->user_data,
                file->path);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = UMI_STATUS_OK;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Remove ai coding checkpoint while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_coding_checkpoint_remove(
    UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || checkpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(store, checkpoint_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position == store->count) return UMI_STATUS_NOT_FOUND;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < store->count) {
        (void)memmove(
            &store->checkpoints[position],
            &store->checkpoints[position + 1U],
            (store->count - position - 1U) *
                sizeof(store->checkpoints[0]));
    }

    store->count -= 1U;
    (void)memset(
        &store->checkpoints[store->count],
        0,
        sizeof(store->checkpoints[0]));
    return UMI_STATUS_OK;
}
