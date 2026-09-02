/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/runtime/scheduler.c
 *
 * PURPOSE:
 *   Implement the scheduler behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/scheduler.h"

#include <stdlib.h>
#include <string.h>

#define UMI_SCHEDULER_CAPACITY 256U

typedef struct UmiScheduledTask {
    uint64_t id;
    uint64_t due_nanoseconds;
    UmiScheduledTaskFn callback;
    void *user_data;
    int active;
} UmiScheduledTask;

struct UmiScheduler {
    UmiClock *clock;
    UmiScheduledTask tasks[UMI_SCHEDULER_CAPACITY];
    uint64_t next_id;
    size_t pending;
};

/*
 * Initialise scheduler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_scheduler_create(UmiClock *clock, UmiScheduler **out_scheduler)
{
    UmiScheduler *scheduler;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock == 0 || clock->monotonic_nanoseconds == 0 || out_scheduler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    scheduler = (UmiScheduler *)calloc(1U, sizeof(*scheduler));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (scheduler == 0) return UMI_STATUS_OUT_OF_MEMORY;
    scheduler->clock = clock;
    scheduler->next_id = 1U;
    *out_scheduler = scheduler;
    return UMI_STATUS_OK;
}

/* Release or reset state held by scheduler so the same storage can be reused safely. */
void umi_scheduler_destroy(UmiScheduler *scheduler)
{
    free(scheduler);
}

/*
 * Provide the scheduler schedule operation used by this module and its client
 * applications.
 */
UmiStatus umi_scheduler_schedule(UmiScheduler *scheduler, uint64_t due_nanoseconds,
                                 UmiScheduledTaskFn task, void *user_data,
                                 uint64_t *out_task_id)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (scheduler == 0 || task == 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_SCHEDULER_CAPACITY; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!scheduler->tasks[index].active) {
            UmiScheduledTask *entry = &scheduler->tasks[index];
            entry->id = scheduler->next_id++;
            entry->due_nanoseconds = due_nanoseconds;
            entry->callback = task;
            entry->user_data = user_data;
            entry->active = 1;
            scheduler->pending += 1U;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (out_task_id != 0) *out_task_id = entry->id;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the scheduler cancel operation used by this module and its client applications. */
UmiStatus umi_scheduler_cancel(UmiScheduler *scheduler, uint64_t task_id)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (scheduler == 0 || task_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_SCHEDULER_CAPACITY; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (scheduler->tasks[index].active && scheduler->tasks[index].id == task_id) {
            memset(&scheduler->tasks[index], 0, sizeof(scheduler->tasks[index]));
            scheduler->pending -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Provide the scheduler run due operation used by this module and its client applications. */
size_t umi_scheduler_run_due(UmiScheduler *scheduler)
{
    uint64_t now;
    size_t index;
    size_t ran = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (scheduler == 0 || scheduler->clock == 0) return 0U;
    now = scheduler->clock->monotonic_nanoseconds(scheduler->clock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_SCHEDULER_CAPACITY; ++index) {
        UmiScheduledTask entry = scheduler->tasks[index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (entry.active && entry.due_nanoseconds <= now) {
            memset(&scheduler->tasks[index], 0, sizeof(scheduler->tasks[index]));
            scheduler->pending -= 1U;
            entry.callback(entry.id, entry.user_data);
            ran += 1U;
        }
    }
    return ran;
}

/* Provide the scheduler pending operation used by this module and its client applications. */
size_t umi_scheduler_pending(const UmiScheduler *scheduler)
{
    return scheduler != 0 ? scheduler->pending : 0U;
}
