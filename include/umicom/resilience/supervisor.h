/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/supervisor.h
 *
 * PURPOSE:
 *   Supervise bounded component failure, restart and degraded-state decisions
 *   without owning product business logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_SUPERVISOR_H
#define UMICOM_RESILIENCE_SUPERVISOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/resilience/restart_policy.h"
#include "umicom/resilience/fault.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RESILIENCE_COMPONENT_ID_CAPACITY 128U
#define UMI_RESILIENCE_SUPERVISOR_MAX 256U
/**
 * List the named supervised state values accepted by this public contract.
 */
typedef enum UmiSupervisedState { UMI_SUPERVISED_STOPPED = 0, UMI_SUPERVISED_RUNNING = 1, UMI_SUPERVISED_DEGRADED = 2, UMI_SUPERVISED_FAILED = 3 } UmiSupervisedState;
/**
 * Represent the supervised snapshot data shared with callers of this public contract.
 */
typedef struct UmiSupervisedSnapshot { char component_id[UMI_RESILIENCE_COMPONENT_ID_CAPACITY]; UmiSupervisedState state; uint32_t failures; uint32_t restarts; UmiStatus last_status; } UmiSupervisedSnapshot;
/**
 * Represent the resilience supervisor data shared with callers of this public contract.
 */
typedef struct UmiResilienceSupervisor UmiResilienceSupervisor;
/**
 * Initialise resilience supervisor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_resilience_supervisor_create(UmiResilienceSupervisor **out_supervisor);
/**
 * Release or reset state held by resilience supervisor so the same storage can be reused
 * safely.
 */
void umi_resilience_supervisor_destroy(UmiResilienceSupervisor *supervisor);
/**
 * Add resilience supervisor only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_resilience_supervisor_register(UmiResilienceSupervisor *supervisor,
                                             const char *component_id,
                                             const UmiRestartPolicy *policy,
                                             uint64_t now_ns);
/**
 * Provide the resilience supervisor success operation used by this module and its client
 * applications.
 */
UmiStatus umi_resilience_supervisor_success(UmiResilienceSupervisor *supervisor,
                                            const char *component_id,
                                            uint64_t now_ns);
/**
 * Provide the resilience supervisor failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_resilience_supervisor_failure(UmiResilienceSupervisor *supervisor,
                                            const char *component_id,
                                            UmiStatus failure,
                                            uint64_t now_ns,
                                            UmiRestartDecision *out_decision);
/**
 * Return the number of records represented by resilience supervisor without changing their
 * state.
 */
size_t umi_resilience_supervisor_count(const UmiResilienceSupervisor *supervisor);
/**
 * Find resilience supervisor while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_resilience_supervisor_at(const UmiResilienceSupervisor *supervisor,
                                       size_t index,
                                       UmiSupervisedSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif

#endif
