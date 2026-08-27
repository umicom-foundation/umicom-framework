/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/cloud_queue.c
 *
 * PURPOSE:
 *   Implement cloud queue/topic inventory and approval-gated removal planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/cloud_queue.h"

#include <ctype.h>
#include <string.h>

static size_t queue_index(
    const UmiWebWorkbenchCloudQueueModel *model,
    const char *queue_id)
{
    size_t index;
    for (index = 0U; index < model->queue_count; ++index) {
        if (strcmp(model->queues[index].queue_id, queue_id) == 0) return index;
    }
    return model->queue_count;
}

static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    length = strlen(query);
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        for (index = 0U; index < length; ++index) {
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        if (index == length) return 1;
    }
    return 0;
}

void umi_web_workbench_cloud_queue_model_init(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudProfile *profile)
{
    if (model == NULL) return;
    memset(model, 0, sizeof(*model));
    if (profile != NULL) model->profile = *profile;
    model->revision = 1U;
}

UmiStatus umi_web_workbench_cloud_queue_upsert(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudQueue *queue)
{
    size_t index;
    if (model == NULL || queue == NULL || queue->queue_id[0] == '\0' ||
        queue->name[0] == '\0' || queue->kind > UMI_WEB_WORKBENCH_CLOUD_DEAD_LETTER) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = queue_index(model, queue->queue_id);
    if (index == model->queue_count) {
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

UmiStatus umi_web_workbench_cloud_queue_remove(
    UmiWebWorkbenchCloudQueueModel *model,
    const char *queue_id,
    bool approved)
{
    size_t index;
    if (model == NULL || queue_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    index = queue_index(model, queue_id);
    if (index == model->queue_count) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < model->queue_count) {
        memmove(&model->queues[index], &model->queues[index + 1U],
            (model->queue_count - index - 1U) * sizeof(model->queues[0]));
    }
    model->queue_count--;
    memset(&model->queues[model->queue_count], 0, sizeof(model->queues[0]));
    model->revision++;
    return UMI_STATUS_OK;
}

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
    if (model == NULL || (out_queues == NULL && capacity > 0U)) return 0U;
    for (index = 0U; index < model->queue_count && count < capacity; ++index) {
        const UmiWebWorkbenchCloudQueue *queue = &model->queues[index];
        if (filter_kind && queue->kind != kind) continue;
        if (!contains_case_insensitive(queue->name, text) &&
            !contains_case_insensitive(queue->queue_id, text)) continue;
        out_queues[count++] = queue;
    }
    return count;
}
