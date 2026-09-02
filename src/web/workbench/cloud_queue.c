/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/cloud_queue.c
 *
 * PURPOSE:
 *   Implement cloud queue/topic inventory and approval-gated removal planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/cloud_queue.h"

#include <ctype.h>
#include <string.h>

/* Provide the queue index operation used by this module and its client applications. */
static size_t queue_index(
    const UmiWebWorkbenchCloudQueueModel *model,
    const char *queue_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->queue_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->queues[index].queue_id, queue_id) == 0) return index;
    }
    return model->queue_count;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}

/*
 * Initialise web workbench cloud queue model from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_cloud_queue_model_init(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudProfile *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    memset(model, 0, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile != NULL) model->profile = *profile;
    model->revision = 1U;
}

/*
 * Provide the web workbench cloud queue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_cloud_queue_upsert(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudQueue *queue)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || queue == NULL || queue->queue_id[0] == '\0' ||
        queue->name[0] == '\0' || queue->kind > UMI_WEB_WORKBENCH_CLOUD_DEAD_LETTER) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = queue_index(model, queue->queue_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == model->queue_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_WORKBENCH_MAX_CLOUD_ITEMS) {
            model->omitted_count++;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        model->queue_count++;
    }
    model->queues[index] = *queue;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Remove web workbench cloud queue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_cloud_queue_remove(
    UmiWebWorkbenchCloudQueueModel *model,
    const char *queue_id,
    bool approved)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || queue_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    index = queue_index(model, queue_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == model->queue_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->queue_count) {
        memmove(&model->queues[index], &model->queues[index + 1U],
            (model->queue_count - index - 1U) * sizeof(model->queues[0]));
    }
    model->queue_count--;
    memset(&model->queues[model->queue_count], 0, sizeof(model->queues[0]));
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench cloud queue query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_cloud_queue_query(
    const UmiWebWorkbenchCloudQueueModel *model,
    const char *text,
    UmiWebWorkbenchCloudQueueKind kind,
    bool filter_kind,
    const UmiWebWorkbenchCloudQueue **out_queues,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || (out_queues == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->queue_count && count < capacity; ++index) {
        const UmiWebWorkbenchCloudQueue *queue = &model->queues[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (filter_kind && queue->kind != kind) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_case_insensitive(queue->name, text) &&
            !contains_case_insensitive(queue->queue_id, text)) continue;
        out_queues[count++] = queue;
    }
    return count;
}
