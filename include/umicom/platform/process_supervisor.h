/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process_supervisor.h
 *
 * PURPOSE:
 *   Supervise multiple asynchronous child-process jobs with cancellation,
 *   timeouts, captured output, snapshots and deterministic shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_PROCESS_SUPERVISOR_H
#define UMICOM_PLATFORM_PROCESS_SUPERVISOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROCESS_SUPERVISOR_MAX_JOBS 64U
#define UMI_PROCESS_JOB_LABEL_CAPACITY 160U
#define UMI_PROCESS_JOB_OUTPUT_CAPACITY 4096U

typedef uint64_t UmiProcessJobId;

typedef enum UmiProcessJobState {
    UMI_PROCESS_JOB_CREATED = 0,
    UMI_PROCESS_JOB_RUNNING = 1,
    UMI_PROCESS_JOB_SUCCEEDED = 2,
    UMI_PROCESS_JOB_FAILED = 3,
    UMI_PROCESS_JOB_CANCELLED = 4,
    UMI_PROCESS_JOB_TIMED_OUT = 5
} UmiProcessJobState;

typedef struct UmiProcessSupervisorConfig {
    size_t capacity;
} UmiProcessSupervisorConfig;

typedef struct UmiProcessJobSnapshot {
    UmiProcessJobId job_id;
    char label[UMI_PROCESS_JOB_LABEL_CAPACITY];
    UmiProcessJobState state;
    int exit_code;
    uint64_t duration_ms;
    int output_truncated;
    char output[UMI_PROCESS_JOB_OUTPUT_CAPACITY];
} UmiProcessJobSnapshot;

typedef struct UmiProcessSupervisorStats {
    size_t capacity;
    size_t jobs;
    size_t running;
    uint64_t submitted;
    uint64_t succeeded;
    uint64_t failed;
    uint64_t cancelled;
    uint64_t timed_out;
} UmiProcessSupervisorStats;

typedef struct UmiProcessSupervisor UmiProcessSupervisor;

UmiProcessSupervisorConfig umi_process_supervisor_config_default(void);
UmiStatus umi_process_supervisor_create(
    const UmiProcessSupervisorConfig *config,
    UmiProcessSupervisor **out_supervisor
);
void umi_process_supervisor_destroy(UmiProcessSupervisor *supervisor);
UmiStatus umi_process_supervisor_submit(UmiProcessSupervisor *supervisor,
                                        const char *label,
                                        const UmiProcessRequest *request,
                                        UmiProcessJobId *out_job_id);
UmiStatus umi_process_supervisor_cancel(UmiProcessSupervisor *supervisor,
                                        UmiProcessJobId job_id);
UmiStatus umi_process_supervisor_wait(UmiProcessSupervisor *supervisor,
                                      UmiProcessJobId job_id,
                                      uint32_t timeout_ms);
UmiStatus umi_process_supervisor_snapshot(
    const UmiProcessSupervisor *supervisor,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot
);
UmiStatus umi_process_supervisor_at(const UmiProcessSupervisor *supervisor,
                                    size_t index,
                                    UmiProcessJobSnapshot *out_snapshot);
UmiStatus umi_process_supervisor_shutdown(UmiProcessSupervisor *supervisor);
UmiProcessSupervisorStats umi_process_supervisor_stats(
    const UmiProcessSupervisor *supervisor
);
const char *umi_process_job_state_text(UmiProcessJobState state);

#ifdef __cplusplus
}
#endif

#endif
