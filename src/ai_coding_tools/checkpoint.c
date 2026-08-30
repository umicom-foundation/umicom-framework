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

static size_t find_index(
    const UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id)
{
    size_t index;

    for (index = 0U; index < store->count; ++index) {
        if (strcmp(
                store->checkpoints[index].checkpoint_id,
                checkpoint_id) == 0) {
            return index;
        }
    }

    return store->count;
}

void umi_ai_coding_checkpoint_store_init(UmiAiCodingCheckpointStore *store)
{
    if (store != NULL) (void)memset(store, 0, sizeof(*store));
}

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

    if (store == NULL || workspace == NULL ||
        checkpoint_id == NULL || checkpoint_id[0] == '\0' ||
        label == NULL || paths == NULL || path_count == 0U ||
        path_count > UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&checkpoint, 0, sizeof(checkpoint));

    if (strlen(checkpoint_id) >= sizeof(checkpoint.checkpoint_id) ||
        strlen(label) >= sizeof(checkpoint.label)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(checkpoint.checkpoint_id, checkpoint_id);
    (void)strcpy(checkpoint.label, label);

    for (index = 0U; index < path_count; ++index) {
        UmiAiCodingCheckpointFile *file =
            &checkpoint.files[checkpoint.file_count];
        int exists = 0;
        size_t length = 0U;

        if (paths[index] == NULL ||
            strlen(paths[index]) >= sizeof(file->path)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        status = workspace->exists(
            workspace->user_data,
            paths[index],
            &exists);
        if (status != UMI_STATUS_OK) return status;

        (void)strcpy(file->path, paths[index]);
        file->existed = exists;

        if (exists) {
            status = workspace->read(
                workspace->user_data,
                paths[index],
                file->text,
                sizeof(file->text),
                &length);
            if (status != UMI_STATUS_OK) return status;

            file->length = length;
            file->hash = umi_ai_coding_text_hash(file->text, length);
        }

        checkpoint.file_count += 1U;
    }

    checkpoint.revision = ++store->sequence;

    position = find_index(store, checkpoint_id);

    if (position == store->count) {
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

UmiStatus umi_ai_coding_checkpoint_find(
    const UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id,
    UmiAiCodingCheckpoint *out_checkpoint)
{
    size_t position;

    if (store == NULL || checkpoint_id == NULL ||
        out_checkpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(store, checkpoint_id);
    if (position == store->count) return UMI_STATUS_NOT_FOUND;

    *out_checkpoint = store->checkpoints[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_checkpoint_restore(
    UmiAiCodingCheckpointStore *store,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *checkpoint_id)
{
    UmiAiCodingCheckpoint checkpoint;
    size_t index;
    UmiStatus status;

    if (store == NULL || workspace == NULL ||
        checkpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_checkpoint_find(
        store, checkpoint_id, &checkpoint);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < checkpoint.file_count; ++index) {
        const UmiAiCodingCheckpointFile *file =
            &checkpoint.files[index];
        int exists = 0;

        status = workspace->exists(
            workspace->user_data,
            file->path,
            &exists);
        if (status != UMI_STATUS_OK) return status;

        if (file->existed) {
            status = workspace->write(
                workspace->user_data,
                file->path,
                file->text,
                file->length);
        } else if (exists) {
            status = workspace->remove(
                workspace->user_data,
                file->path);
        } else {
            status = UMI_STATUS_OK;
        }

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_checkpoint_remove(
    UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id)
{
    size_t position;

    if (store == NULL || checkpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(store, checkpoint_id);
    if (position == store->count) return UMI_STATUS_NOT_FOUND;

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
