/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/restart_policy.c
 *
 * PURPOSE:
 *   Implement rolling-window restart limits and backoff decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/restart_policy.h"

#include <stddef.h>
UmiRestartPolicy umi_restart_policy_default(void)
{ UmiRestartPolicy p; p.maximum_restarts = 5U; p.window_ns = UINT64_C(60000000000); p.backoff = umi_backoff_policy_default(); return p; }
UmiStatus umi_restart_tracker_init(UmiRestartTracker *tracker, const UmiRestartPolicy *policy, uint64_t now_ns)
{ if (tracker == NULL) return UMI_STATUS_INVALID_ARGUMENT; tracker->policy = policy != NULL ? *policy : umi_restart_policy_default(); if (tracker->policy.maximum_restarts == 0U || tracker->policy.window_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT; tracker->restarts = 0U; tracker->window_started_ns = now_ns; return UMI_STATUS_OK; }
UmiRestartDecision umi_restart_tracker_failure(UmiRestartTracker *tracker, uint64_t now_ns)
{
    UmiRestartDecision d = {0,0U,0U};
    if (tracker == NULL) return d;
    if (now_ns - tracker->window_started_ns >= tracker->policy.window_ns) { tracker->window_started_ns = now_ns; tracker->restarts = 0U; }
    if (tracker->restarts >= tracker->policy.maximum_restarts) return d;
    d.restart = 1; d.restart_number = ++tracker->restarts; d.delay_ms = umi_backoff_delay(&tracker->policy.backoff, d.restart_number - 1U, d.restart_number); return d;
}
void umi_restart_tracker_success(UmiRestartTracker *tracker, uint64_t now_ns)
{ if (tracker != NULL) { tracker->restarts = 0U; tracker->window_started_ns = now_ns; } }
