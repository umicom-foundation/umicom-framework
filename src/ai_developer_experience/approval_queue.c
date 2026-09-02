/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/approval_queue.c
 *
 * PURPOSE:
 *   Implement bounded approval storage with stable identity and revisioning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/approval_queue.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperApprovalQueue {
    UmiAiDeveloperApprovalRequest items[UMI_AI_DEVELOPER_APPROVAL_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < queue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(queue->items[index].approval_id, approval_id) == 0) {
            return index;
        }
    }

    return queue->count;
}

/*
 * Initialise ai developer approval queue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_approval_queue_create(
    UmiAiDeveloperApprovalQueue **out_queue)
{
    UmiAiDeveloperApprovalQueue *queue;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiDeveloperApprovalQueue *)calloc(1U, sizeof(*queue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    queue->revision = 1U;
    *out_queue = queue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai developer approval queue so the same storage can be
 * reused safely.
 */
void umi_ai_developer_approval_queue_destroy(
    UmiAiDeveloperApprovalQueue *queue)
{
    free(queue);
}

/*
 * Add ai developer approval queue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_developer_approval_queue_add(
    UmiAiDeveloperApprovalQueue *queue,
    const UmiAiDeveloperApprovalRequest *request)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || request == NULL ||
        request->approval_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, request->approval_id);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == queue->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (queue->count >= UMI_AI_DEVELOPER_APPROVAL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        queue->count += 1U;
    }

    queue->items[index] = *request;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ai developer approval queue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_developer_approval_queue_find(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalRequest *out_request)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || approval_id == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, approval_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == queue->count) return UMI_STATUS_NOT_FOUND;

    *out_request = queue->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find ai developer approval queue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_developer_approval_queue_at(
    const UmiAiDeveloperApprovalQueue *queue,
    size_t index,
    UmiAiDeveloperApprovalRequest *out_request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= queue->count) return UMI_STATUS_NOT_FOUND;

    *out_request = queue->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer approval queue set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_approval_queue_set_state(
    UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalState state)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || approval_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(queue, approval_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == queue->count) return UMI_STATUS_NOT_FOUND;

    queue->items[index].state = state;
    queue->items[index].decided_sequence = queue->revision + 1U;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai developer approval queue without changing
 * their state.
 */
size_t umi_ai_developer_approval_queue_count(
    const UmiAiDeveloperApprovalQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}

/*
 * Return the number of records represented by ai developer approval queue pending without
 * changing their state.
 */
size_t umi_ai_developer_approval_queue_pending_count(
    const UmiAiDeveloperApprovalQueue *queue)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < queue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (queue->items[index].state == UMI_AI_DEVELOPER_APPROVAL_PENDING) {
            count += 1U;
        }
    }

    return count;
}

/*
 * Provide the ai developer approval queue revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_developer_approval_queue_revision(
    const UmiAiDeveloperApprovalQueue *queue)
{
    return queue != NULL ? queue->revision : 0U;
}

/*
 * Release or reset state held by ai developer approval queue so the same storage can be
 * reused safely.
 */
void umi_ai_developer_approval_queue_clear(
    UmiAiDeveloperApprovalQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;

    (void)memset(queue->items, 0, sizeof(queue->items));
    queue->count = 0U;
    queue->revision += 1U;
}
