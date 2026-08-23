/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/task_queue.h
 *
 * PURPOSE:
 *   Queue bounded coding requests by value so a future Studio background worker,
 *   headless agent or remote development host can serialize coding work.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_TASK_QUEUE_H
#define UMICOM_AI_CODING_RUNTIME_TASK_QUEUE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TASK_QUEUE_CAPACITY 32U

typedef struct UmiAiCodingQueuedTask {
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    UmiAiCodingRequest request;
    uint64_t sequence;
} UmiAiCodingQueuedTask;

typedef struct UmiAiCodingTaskQueue UmiAiCodingTaskQueue;

UmiStatus umi_ai_coding_task_queue_create(UmiAiCodingTaskQueue **out_queue);
void umi_ai_coding_task_queue_destroy(UmiAiCodingTaskQueue *queue);
UmiStatus umi_ai_coding_task_queue_push(
    UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *task);
UmiStatus umi_ai_coding_task_queue_pop(
    UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task);
UmiStatus umi_ai_coding_task_queue_peek(
    const UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task);
size_t umi_ai_coding_task_queue_count(const UmiAiCodingTaskQueue *queue);
void umi_ai_coding_task_queue_clear(UmiAiCodingTaskQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
