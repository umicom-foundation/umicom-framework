/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/restart_policy.h
 *
 * PURPOSE:
 *   Define bounded restart decisions over rolling failure windows for supervised
 *   services and workers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_RESTART_POLICY_H
#define UMICOM_RESILIENCE_RESTART_POLICY_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/resilience/backoff.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRestartPolicy { uint32_t maximum_restarts; uint64_t window_ns; UmiBackoffPolicy backoff; } UmiRestartPolicy;
typedef struct UmiRestartTracker { UmiRestartPolicy policy; uint32_t restarts; uint64_t window_started_ns; } UmiRestartTracker;
typedef struct UmiRestartDecision { int restart; uint32_t delay_ms; uint32_t restart_number; } UmiRestartDecision;
UmiRestartPolicy umi_restart_policy_default(void);
UmiStatus umi_restart_tracker_init(UmiRestartTracker *tracker,
                                   const UmiRestartPolicy *policy,
                                   uint64_t now_ns);
UmiRestartDecision umi_restart_tracker_failure(UmiRestartTracker *tracker,
                                               uint64_t now_ns);
void umi_restart_tracker_success(UmiRestartTracker *tracker, uint64_t now_ns);
#ifdef __cplusplus
}
#endif

#endif
