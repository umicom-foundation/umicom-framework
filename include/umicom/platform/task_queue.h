/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/task_queue.h
 *
 * PURPOSE:
 *   Provide a bounded worker queue for cancellable Framework tasks with
 *   deterministic shutdown, idle waiting and operational statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_TASK_QUEUE_H
#define UMICOM_PLATFORM_TASK_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the task queue data shared with callers of this public contract.
 */
typedef struct UmiTaskQueue UmiTaskQueue;

/**
 * Represent the task queue config data shared with callers of this public contract.
 */
typedef struct UmiTaskQueueConfig {
    size_t worker_count;
    size_t capacity;
} UmiTaskQueueConfig;

/**
 * Represent the task queue stats data shared with callers of this public contract.
 */
typedef struct UmiTaskQueueStats {
    size_t worker_count;
    size_t capacity;
    size_t queued;
    size_t running;
    uint64_t submitted;
    uint64_t completed;
    uint64_t cancelled;
    uint64_t failed;
} UmiTaskQueueStats;

/**
 * Provide the task queue config default operation used by this module and its client
 * applications.
 */
UmiTaskQueueConfig umi_task_queue_config_default(void);
/**
 * Initialise task queue from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_task_queue_create(const UmiTaskQueueConfig *config,
                                UmiTaskQueue **out_queue);
/**
 * Release or reset state held by task queue so the same storage can be reused safely.
 */
void umi_task_queue_destroy(UmiTaskQueue *queue);
/**
 * Provide the task queue submit operation used by this module and its client applications.
 */
UmiStatus umi_task_queue_submit(UmiTaskQueue *queue, UmiTask *task);
/**
 * Provide the task queue wait idle operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_queue_wait_idle(UmiTaskQueue *queue, uint32_t timeout_ms);
/**
 * Provide the task queue shutdown operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_queue_shutdown(UmiTaskQueue *queue, int cancel_pending);
/**
 * Provide the task queue stats operation used by this module and its client applications.
 */
UmiTaskQueueStats umi_task_queue_stats(const UmiTaskQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
