/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/scheduler.h
 *
 * PURPOSE:
 *   Publish the public scheduler contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_SCHEDULER_H
#define UMICOM_RUNTIME_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/clock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the scheduler data shared with callers of this public contract.
 */
typedef struct UmiScheduler UmiScheduler;
typedef void (*UmiScheduledTaskFn)(uint64_t task_id, void *user_data);

/**
 * Initialise scheduler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_scheduler_create(UmiClock *clock, UmiScheduler **out_scheduler);
/**
 * Release or reset state held by scheduler so the same storage can be reused safely.
 */
void umi_scheduler_destroy(UmiScheduler *scheduler);
/**
 * Provide the scheduler schedule operation used by this module and its client
 * applications.
 */
UmiStatus umi_scheduler_schedule(UmiScheduler *scheduler, uint64_t due_nanoseconds,
                                 UmiScheduledTaskFn task, void *user_data,
                                 uint64_t *out_task_id);
/**
 * Provide the scheduler cancel operation used by this module and its client applications.
 */
UmiStatus umi_scheduler_cancel(UmiScheduler *scheduler, uint64_t task_id);
/**
 * Provide the scheduler run due operation used by this module and its client applications.
 */
size_t umi_scheduler_run_due(UmiScheduler *scheduler);
/**
 * Provide the scheduler pending operation used by this module and its client applications.
 */
size_t umi_scheduler_pending(const UmiScheduler *scheduler);

#ifdef __cplusplus
}
#endif

#endif
