/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/task_queue.h
 *
 * PURPOSE:
 *   Queue bounded coding requests by value so a future Studio background worker,
 *   headless agent or remote development host can serialize coding work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_TASK_QUEUE_H
#define UMICOM_AI_CODING_RUNTIME_TASK_QUEUE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TASK_QUEUE_CAPACITY 32U

/**
 * Represent the ai coding queued task data shared with callers of this public contract.
 */
typedef struct UmiAiCodingQueuedTask {
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    UmiAiCodingRequest request;
    uint64_t sequence;
} UmiAiCodingQueuedTask;

/**
 * Represent the ai coding task queue data shared with callers of this public contract.
 */
typedef struct UmiAiCodingTaskQueue UmiAiCodingTaskQueue;

/**
 * Initialise ai coding task queue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_coding_task_queue_create(UmiAiCodingTaskQueue **out_queue);
/**
 * Release or reset state held by ai coding task queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_task_queue_destroy(UmiAiCodingTaskQueue *queue);
/**
 * Provide the ai coding task queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_task_queue_push(
    UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *task);
/**
 * Provide the ai coding task queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_task_queue_pop(
    UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task);
/**
 * Provide the ai coding task queue peek operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_task_queue_peek(
    const UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task);
/**
 * Return the number of records represented by ai coding task queue without changing their
 * state.
 */
size_t umi_ai_coding_task_queue_count(const UmiAiCodingTaskQueue *queue);
/**
 * Release or reset state held by ai coding task queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_task_queue_clear(UmiAiCodingTaskQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
