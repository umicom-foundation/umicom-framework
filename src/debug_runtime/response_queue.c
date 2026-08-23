/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/response_queue.c
 *
 * PURPOSE:
 *   Implement heap-owned out-of-order DAP response preservation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/response_queue.h"
#include <stdlib.h>
#include <string.h>

struct UmiDebugRuntimeResponseQueue {
    UmiDebugRuntimeEnvelope
        items[UMI_DEBUG_RUNTIME_MAX_DEFERRED_RESPONSES];
    size_t count;
    uint64_t revision;
};

UmiStatus umi_debug_runtime_response_queue_create(
    UmiDebugRuntimeResponseQueue **out_queue)
{
    UmiDebugRuntimeResponseQueue *queue;

    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiDebugRuntimeResponseQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    queue->revision = 1U;
    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_debug_runtime_response_queue_destroy(
    UmiDebugRuntimeResponseQueue *queue)
{
    free(queue);
}

UmiStatus umi_debug_runtime_response_queue_push(
    UmiDebugRuntimeResponseQueue *queue,
    const UmiDebugRuntimeEnvelope *response)
{
    if (queue == NULL || response == NULL ||
        response->kind != UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE ||
        response->request_sequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count >= UMI_DEBUG_RUNTIME_MAX_DEFERRED_RESPONSES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    queue->items[queue->count++] = *response;
    queue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_response_queue_take(
    UmiDebugRuntimeResponseQueue *queue,
    uint64_t request_sequence,
    UmiDebugRuntimeEnvelope *out_response)
{
    size_t index;

    if (queue == NULL || request_sequence == 0U ||
        out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < queue->count; ++index) {
        if (queue->items[index].request_sequence == request_sequence) {
            const size_t tail = queue->count - index - 1U;

            *out_response = queue->items[index];

            if (tail > 0U) {
                (void)memmove(
                    &queue->items[index],
                    &queue->items[index + 1U],
                    tail * sizeof(queue->items[0]));
            }

            queue->count -= 1U;
            (void)memset(
                &queue->items[queue->count],
                0,
                sizeof(queue->items[0]));
            queue->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

size_t umi_debug_runtime_response_queue_count(
    const UmiDebugRuntimeResponseQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}
