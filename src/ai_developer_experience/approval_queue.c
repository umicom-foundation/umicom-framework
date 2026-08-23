/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/approval_queue.c
 *
 * PURPOSE:
 *   Implement bounded approval storage with stable identity and revisioning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/approval_queue.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperApprovalQueue {
    UmiAiDeveloperApprovalRequest items[UMI_AI_DEVELOPER_APPROVAL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id)
{
    size_t index;

    for (index = 0U; index < queue->count; ++index) {
        if (strcmp(queue->items[index].approval_id, approval_id) == 0) {
            return index;
        }
    }

    return queue->count;
}

UmiStatus umi_ai_developer_approval_queue_create(
    UmiAiDeveloperApprovalQueue **out_queue)
{
    UmiAiDeveloperApprovalQueue *queue;

    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiDeveloperApprovalQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    queue->revision = 1U;
    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_ai_developer_approval_queue_destroy(
    UmiAiDeveloperApprovalQueue *queue)
{
    free(queue);
}

UmiStatus umi_ai_developer_approval_queue_add(
    UmiAiDeveloperApprovalQueue *queue,
    const UmiAiDeveloperApprovalRequest *request)
{
    size_t index;

    if (queue == NULL || request == NULL ||
        request->approval_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, request->approval_id);

    if (index == queue->count) {
        if (queue->count >= UMI_AI_DEVELOPER_APPROVAL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        queue->count += 1U;
    }

    queue->items[index] = *request;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approval_queue_find(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalRequest *out_request)
{
    size_t index;

    if (queue == NULL || approval_id == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, approval_id);
    if (index == queue->count) return UMI_STATUS_NOT_FOUND;

    *out_request = queue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approval_queue_at(
    const UmiAiDeveloperApprovalQueue *queue,
    size_t index,
    UmiAiDeveloperApprovalRequest *out_request)
{
    if (queue == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= queue->count) return UMI_STATUS_NOT_FOUND;

    *out_request = queue->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approval_queue_set_state(
    UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalState state)
{
    size_t index;

    if (queue == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, approval_id);
    if (index == queue->count) return UMI_STATUS_NOT_FOUND;

    queue->items[index].state = state;
    queue->items[index].decided_sequence = queue->revision + 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

size_t umi_ai_developer_approval_queue_count(
    const UmiAiDeveloperApprovalQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}

size_t umi_ai_developer_approval_queue_pending_count(
    const UmiAiDeveloperApprovalQueue *queue)
{
    size_t index;
    size_t count = 0U;

    if (queue == NULL) return 0U;

    for (index = 0U; index < queue->count; ++index) {
        if (queue->items[index].state == UMI_AI_DEVELOPER_APPROVAL_PENDING) {
            count += 1U;
        }
    }

    return count;
}

uint64_t umi_ai_developer_approval_queue_revision(
    const UmiAiDeveloperApprovalQueue *queue)
{
    return queue != NULL ? queue->revision : 0U;
}

void umi_ai_developer_approval_queue_clear(
    UmiAiDeveloperApprovalQueue *queue)
{
    if (queue == NULL) return;

    (void)memset(queue->items, 0, sizeof(queue->items));
    queue->count = 0U;
    queue->revision += 1U;
}
