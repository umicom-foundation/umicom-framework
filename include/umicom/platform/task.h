/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/task.h
 *
 * PURPOSE:
 *   Define a cancellable, observable unit of background work that can run in
 *   the Framework task queue without leaking platform thread primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_TASK_H
#define UMICOM_PLATFORM_TASK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named task state values accepted by this public contract.
 */
typedef enum UmiTaskState {
    UMI_TASK_CREATED = 0,
    UMI_TASK_QUEUED = 1,
    UMI_TASK_RUNNING = 2,
    UMI_TASK_SUCCEEDED = 3,
    UMI_TASK_FAILED = 4,
    UMI_TASK_CANCELLED = 5
} UmiTaskState;

/**
 * Represent the task data shared with callers of this public contract.
 */
typedef struct UmiTask UmiTask;
/**
 * Represent the task context data shared with callers of this public contract.
 */
typedef struct UmiTaskContext UmiTaskContext;
typedef UmiStatus (*UmiTaskFunction)(UmiTaskContext *context,
                                     void *user_data);
typedef void (*UmiTaskProgressSink)(uint64_t task_id,
                                    unsigned progress_percent,
                                    const char *message,
                                    void *user_data);

/**
 * Represent the task config data shared with callers of this public contract.
 */
typedef struct UmiTaskConfig {
    const char *label;
    UmiTaskFunction function;
    void *user_data;
    UmiTaskProgressSink progress_sink;
    void *progress_user_data;
} UmiTaskConfig;

/**
 * Initialise task from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_task_create(const UmiTaskConfig *config,
                          UmiTask **out_task);
/**
 * Release or reset state held by task so the same storage can be reused safely.
 */
void umi_task_destroy(UmiTask *task);
/**
 * Perform task through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_task_run(UmiTask *task);
/**
 * Provide the task mark queued operation used by this module and its client applications.
 */
UmiStatus umi_task_mark_queued(UmiTask *task);
/**
 * Provide the task cancel operation used by this module and its client applications.
 */
UmiStatus umi_task_cancel(UmiTask *task);
/**
 * Provide the task wait operation used by this module and its client applications.
 */
UmiStatus umi_task_wait(UmiTask *task, uint32_t timeout_ms);
/**
 * Provide the task state operation used by this module and its client applications.
 */
UmiTaskState umi_task_state(const UmiTask *task);
/**
 * Provide the task result operation used by this module and its client applications.
 */
UmiStatus umi_task_result(const UmiTask *task);
/**
 * Provide the task id operation used by this module and its client applications.
 */
uint64_t umi_task_id(const UmiTask *task);
/**
 * Provide the task label operation used by this module and its client applications.
 */
const char *umi_task_label(const UmiTask *task);
/**
 * Provide the task progress operation used by this module and its client applications.
 */
unsigned umi_task_progress(const UmiTask *task);

/**
 * Provide the task context is cancelled operation used by this module and its client
 * applications.
 */
int umi_task_context_is_cancelled(const UmiTaskContext *context);
/**
 * Provide the task context report operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_context_report(UmiTaskContext *context,
                                  unsigned progress_percent,
                                  const char *message);
/**
 * Provide the task context id operation used by this module and its client applications.
 */
uint64_t umi_task_context_id(const UmiTaskContext *context);

#ifdef __cplusplus
}
#endif

#endif
