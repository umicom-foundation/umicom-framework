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

typedef enum UmiTaskState {
    UMI_TASK_CREATED = 0,
    UMI_TASK_QUEUED = 1,
    UMI_TASK_RUNNING = 2,
    UMI_TASK_SUCCEEDED = 3,
    UMI_TASK_FAILED = 4,
    UMI_TASK_CANCELLED = 5
} UmiTaskState;

typedef struct UmiTask UmiTask;
typedef struct UmiTaskContext UmiTaskContext;
typedef UmiStatus (*UmiTaskFunction)(UmiTaskContext *context,
                                     void *user_data);
typedef void (*UmiTaskProgressSink)(uint64_t task_id,
                                    unsigned progress_percent,
                                    const char *message,
                                    void *user_data);

typedef struct UmiTaskConfig {
    const char *label;
    UmiTaskFunction function;
    void *user_data;
    UmiTaskProgressSink progress_sink;
    void *progress_user_data;
} UmiTaskConfig;

UmiStatus umi_task_create(const UmiTaskConfig *config,
                          UmiTask **out_task);
void umi_task_destroy(UmiTask *task);
UmiStatus umi_task_run(UmiTask *task);
UmiStatus umi_task_mark_queued(UmiTask *task);
UmiStatus umi_task_cancel(UmiTask *task);
UmiStatus umi_task_wait(UmiTask *task, uint32_t timeout_ms);
UmiTaskState umi_task_state(const UmiTask *task);
UmiStatus umi_task_result(const UmiTask *task);
uint64_t umi_task_id(const UmiTask *task);
const char *umi_task_label(const UmiTask *task);
unsigned umi_task_progress(const UmiTask *task);

int umi_task_context_is_cancelled(const UmiTaskContext *context);
UmiStatus umi_task_context_report(UmiTaskContext *context,
                                  unsigned progress_percent,
                                  const char *message);
uint64_t umi_task_context_id(const UmiTaskContext *context);

#ifdef __cplusplus
}
#endif

#endif
