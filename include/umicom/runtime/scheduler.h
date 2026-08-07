#ifndef UMICOM_RUNTIME_SCHEDULER_H
#define UMICOM_RUNTIME_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/clock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiScheduler UmiScheduler;
typedef void (*UmiScheduledTaskFn)(uint64_t task_id, void *user_data);

UmiStatus umi_scheduler_create(UmiClock *clock, UmiScheduler **out_scheduler);
void umi_scheduler_destroy(UmiScheduler *scheduler);
UmiStatus umi_scheduler_schedule(UmiScheduler *scheduler, uint64_t due_nanoseconds,
                                 UmiScheduledTaskFn task, void *user_data,
                                 uint64_t *out_task_id);
UmiStatus umi_scheduler_cancel(UmiScheduler *scheduler, uint64_t task_id);
size_t umi_scheduler_run_due(UmiScheduler *scheduler);
size_t umi_scheduler_pending(const UmiScheduler *scheduler);

#ifdef __cplusplus
}
#endif

#endif
