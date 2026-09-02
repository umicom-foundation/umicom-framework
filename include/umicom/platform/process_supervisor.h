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

/**
 * List the named process job state values accepted by this public contract.
 */
typedef enum UmiProcessJobState {
    UMI_PROCESS_JOB_CREATED = 0,
    UMI_PROCESS_JOB_RUNNING = 1,
    UMI_PROCESS_JOB_SUCCEEDED = 2,
    UMI_PROCESS_JOB_FAILED = 3,
    UMI_PROCESS_JOB_CANCELLED = 4,
    UMI_PROCESS_JOB_TIMED_OUT = 5
} UmiProcessJobState;

/**
 * Represent the process supervisor config data shared with callers of this public
 * contract.
 */
typedef struct UmiProcessSupervisorConfig {
    size_t capacity;
} UmiProcessSupervisorConfig;

/**
 * Represent the process job snapshot data shared with callers of this public contract.
 */
typedef struct UmiProcessJobSnapshot {
    UmiProcessJobId job_id;
    char label[UMI_PROCESS_JOB_LABEL_CAPACITY];
    UmiProcessJobState state;
    int exit_code;
    uint64_t duration_ms;
    int output_truncated;
    char output[UMI_PROCESS_JOB_OUTPUT_CAPACITY];
} UmiProcessJobSnapshot;

/**
 * Represent the process supervisor stats data shared with callers of this public contract.
 */
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

/**
 * Represent the process supervisor data shared with callers of this public contract.
 */
typedef struct UmiProcessSupervisor UmiProcessSupervisor;

/**
 * Provide the process supervisor config default operation used by this module and its
 * client applications.
 */
UmiProcessSupervisorConfig umi_process_supervisor_config_default(void);
/**
 * Initialise process supervisor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_process_supervisor_create(
    const UmiProcessSupervisorConfig *config,
    UmiProcessSupervisor **out_supervisor
);
/**
 * Release or reset state held by process supervisor so the same storage can be reused
 * safely.
 */
void umi_process_supervisor_destroy(UmiProcessSupervisor *supervisor);
/**
 * Provide the process supervisor submit operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_submit(UmiProcessSupervisor *supervisor,
                                        const char *label,
                                        const UmiProcessRequest *request,
                                        UmiProcessJobId *out_job_id);
/**
 * Provide the process supervisor cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_cancel(UmiProcessSupervisor *supervisor,
                                        UmiProcessJobId job_id);
/**
 * Provide the process supervisor wait operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_wait(UmiProcessSupervisor *supervisor,
                                      UmiProcessJobId job_id,
                                      uint32_t timeout_ms);
/**
 * Provide the process supervisor snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_snapshot(
    const UmiProcessSupervisor *supervisor,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot
);
/**
 * Find process supervisor while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_process_supervisor_at(const UmiProcessSupervisor *supervisor,
                                    size_t index,
                                    UmiProcessJobSnapshot *out_snapshot);
/**
 * Provide the process supervisor shutdown operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_shutdown(UmiProcessSupervisor *supervisor);
/**
 * Provide the process supervisor stats operation used by this module and its client
 * applications.
 */
UmiProcessSupervisorStats umi_process_supervisor_stats(
    const UmiProcessSupervisor *supervisor
);
/**
 * Provide the process job state text operation used by this module and its client
 * applications.
 */
const char *umi_process_job_state_text(UmiProcessJobState state);

#ifdef __cplusplus
}
#endif

#endif
