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
/* On success the queue retains the task until removal/completion; the caller
 * keeps its own reference. Queue-full/stopping rejection leaves a CREATED task
 * reusable instead of cancelling work the queue never accepted. The task must
 * not be run manually after submission. Callback payloads remain caller-owned. */
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

/** Request cancellation of currently queued and running tasks without joining
 * worker threads. The queue remains usable for subsequent submissions. This
 * affects the whole queue: use umi_task_cancel for one selected task, not this
 * function on a shared application queue. Running callbacks must cooperate. */
UmiStatus UmiTaskQueueCancelAll(UmiTaskQueue *queue);
/** Close admission and optionally cancel queued/running work; return without
 * waiting for worker callbacks. Repeated requests can escalate cancellation.
 * After idleness, the lifecycle owner calls shutdown to join, then destroy.
 * Queue destruction must not race any queue call or run on a queue worker.
 * Existing shutdown retains its blocking drain/cancel-pending behaviour. */
UmiStatus UmiTaskQueueRequestShutdown(UmiTaskQueue *queue,
    int cancelPending, int cancelRunning);

/** Phase of the queue's existing lifecycle, not a task outcome. */
typedef enum UmiTaskQueuePhase {
    UMI_TASK_QUEUE_ACCEPTING = 0,
    UMI_TASK_QUEUE_STOP_REQUESTED = 1,
    UMI_TASK_QUEUE_JOINING = 2,
    UMI_TASK_QUEUE_STOPPED = 3
} UmiTaskQueuePhase;

typedef struct UmiTaskQueueShutdownSnapshot {
    UmiTaskQueuePhase phase;
    UmiTaskQueueStats tasks;
    int nonblockingJoinAvailable;
} UmiTaskQueueShutdownSnapshot;

/** Copy a coherent phase and task-counter observation under the queue mutex.
 * Does not request a stop or reload any state. Failure leaves output untouched.
 * STOPPED means every worker was joined; zero running/queued tasks alone does
 * not prove native completion. The plain-value copy owns no queue pointers. */
UmiStatus UmiTaskQueueCaptureShutdown(const UmiTaskQueue *queue,
    UmiTaskQueueShutdownSnapshot *outSnapshot);

/** After RequestShutdown, reap only native workers that have already stopped.
 * Does not wait for worker callbacks or thread-local teardown. BUSY means a
 * worker is still alive or another join pass owns the queue. Repeated passes
 * retain partial join progress. OK is idempotent and means all workers joined.
 * An accepting queue or a call from its worker returns INVALID_STATE. A worker
 * that exits without returning through the task runner is unsupported: do not
 * treat stranded task accounting as successful shutdown. No forced exit.
 * Queue mutex acquisition and OS scheduling are not real-time guarantees. */
UmiStatus UmiTaskQueueTryFinishShutdown(UmiTaskQueue *queue);

/** Release a STOPPED queue and clear the caller pointer on success. Never
 * starts a stop or performs a blocking join. BUSY preserves a non-stopped queue.
 * A native thread-handle release failure preserves the queue for retry; earlier
 * successful handle releases stay released. NULL owned queue is an OK no-op.
 * The lifecycle owner must first exclude ALL concurrent queue users, callbacks,
 * polling sources and aliases. A stopped queue is not a reference-counted
 * lifetime object. Callers retain their separate task references and payloads. */
UmiStatus UmiTaskQueueReleaseStopped(UmiTaskQueue **inOutQueue);

#ifdef __cplusplus
}
#endif

#endif
