/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/restart_policy.h
 *
 * PURPOSE:
 *   Define bounded restart decisions over rolling failure windows for supervised
 *   services and workers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_RESTART_POLICY_H
#define UMICOM_RESILIENCE_RESTART_POLICY_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/resilience/backoff.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the restart policy data shared with callers of this public contract.
 */
typedef struct UmiRestartPolicy { uint32_t maximum_restarts; uint64_t window_ns; UmiBackoffPolicy backoff; } UmiRestartPolicy;
/**
 * Represent the restart tracker data shared with callers of this public contract.
 */
typedef struct UmiRestartTracker { UmiRestartPolicy policy; uint32_t restarts; uint64_t window_started_ns; } UmiRestartTracker;
/**
 * Represent the restart decision data shared with callers of this public contract.
 */
typedef struct UmiRestartDecision { int restart; uint32_t delay_ms; uint32_t restart_number; } UmiRestartDecision;
/**
 * Provide the restart policy default operation used by this module and its client
 * applications.
 */
UmiRestartPolicy umi_restart_policy_default(void);
/**
 * Initialise restart tracker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_restart_tracker_init(UmiRestartTracker *tracker,
                                   const UmiRestartPolicy *policy,
                                   uint64_t now_ns);
/**
 * Provide the restart tracker failure operation used by this module and its client
 * applications.
 */
UmiRestartDecision umi_restart_tracker_failure(UmiRestartTracker *tracker,
                                               uint64_t now_ns);
/**
 * Provide the restart tracker success operation used by this module and its client
 * applications.
 */
void umi_restart_tracker_success(UmiRestartTracker *tracker, uint64_t now_ns);
#ifdef __cplusplus
}
#endif

#endif
