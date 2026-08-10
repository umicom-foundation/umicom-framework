/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/task_queue.h
 *
 * PURPOSE:
 *   Provide a bounded worker queue for cancellable Framework tasks with
 *   deterministic shutdown, idle waiting and operational statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTaskQueue UmiTaskQueue;

typedef struct UmiTaskQueueConfig {
    size_t worker_count;
    size_t capacity;
} UmiTaskQueueConfig;

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

UmiTaskQueueConfig umi_task_queue_config_default(void);
UmiStatus umi_task_queue_create(const UmiTaskQueueConfig *config,
                                UmiTaskQueue **out_queue);
void umi_task_queue_destroy(UmiTaskQueue *queue);
UmiStatus umi_task_queue_submit(UmiTaskQueue *queue, UmiTask *task);
UmiStatus umi_task_queue_wait_idle(UmiTaskQueue *queue, uint32_t timeout_ms);
UmiStatus umi_task_queue_shutdown(UmiTaskQueue *queue, int cancel_pending);
UmiTaskQueueStats umi_task_queue_stats(const UmiTaskQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
