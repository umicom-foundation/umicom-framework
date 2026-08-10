/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/supervisor.h
 *
 * PURPOSE:
 *   Supervise bounded component failure, restart and degraded-state decisions
 *   without owning product business logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiSupervisedState { UMI_SUPERVISED_STOPPED = 0, UMI_SUPERVISED_RUNNING = 1, UMI_SUPERVISED_DEGRADED = 2, UMI_SUPERVISED_FAILED = 3 } UmiSupervisedState;
typedef struct UmiSupervisedSnapshot { char component_id[UMI_RESILIENCE_COMPONENT_ID_CAPACITY]; UmiSupervisedState state; uint32_t failures; uint32_t restarts; UmiStatus last_status; } UmiSupervisedSnapshot;
typedef struct UmiResilienceSupervisor UmiResilienceSupervisor;
UmiStatus umi_resilience_supervisor_create(UmiResilienceSupervisor **out_supervisor);
void umi_resilience_supervisor_destroy(UmiResilienceSupervisor *supervisor);
UmiStatus umi_resilience_supervisor_register(UmiResilienceSupervisor *supervisor,
                                             const char *component_id,
                                             const UmiRestartPolicy *policy,
                                             uint64_t now_ns);
UmiStatus umi_resilience_supervisor_success(UmiResilienceSupervisor *supervisor,
                                            const char *component_id,
                                            uint64_t now_ns);
UmiStatus umi_resilience_supervisor_failure(UmiResilienceSupervisor *supervisor,
                                            const char *component_id,
                                            UmiStatus failure,
                                            uint64_t now_ns,
                                            UmiRestartDecision *out_decision);
size_t umi_resilience_supervisor_count(const UmiResilienceSupervisor *supervisor);
UmiStatus umi_resilience_supervisor_at(const UmiResilienceSupervisor *supervisor,
                                       size_t index,
                                       UmiSupervisedSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif

#endif
