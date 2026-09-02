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
/*
 * Provide the restart policy default operation used by this module and its client
 * applications.
 */
UmiRestartPolicy umi_restart_policy_default(void)
{ UmiRestartPolicy p; p.maximum_restarts = 5U; p.window_ns = UINT64_C(60000000000); p.backoff = umi_backoff_policy_default(); return p; }
/*
 * Initialise restart tracker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_restart_tracker_init(UmiRestartTracker *tracker, const UmiRestartPolicy *policy, uint64_t now_ns)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tracker == NULL) return UMI_STATUS_INVALID_ARGUMENT; tracker->policy = policy != NULL ? *policy : umi_restart_policy_default(); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tracker->policy.maximum_restarts == 0U || tracker->policy.window_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT; tracker->restarts = 0U; tracker->window_started_ns = now_ns; return UMI_STATUS_OK; }
/*
 * Provide the restart tracker failure operation used by this module and its client
 * applications.
 */
UmiRestartDecision umi_restart_tracker_failure(UmiRestartTracker *tracker, uint64_t now_ns)
{
    UmiRestartDecision d = {0,0U,0U};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tracker == NULL) return d;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (now_ns - tracker->window_started_ns >= tracker->policy.window_ns) { tracker->window_started_ns = now_ns; tracker->restarts = 0U; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tracker->restarts >= tracker->policy.maximum_restarts) return d;
    d.restart = 1; d.restart_number = ++tracker->restarts; d.delay_ms = umi_backoff_delay(&tracker->policy.backoff, d.restart_number - 1U, d.restart_number); return d;
}
/*
 * Provide the restart tracker success operation used by this module and its client
 * applications.
 */
void umi_restart_tracker_success(UmiRestartTracker *tracker, uint64_t now_ns)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tracker != NULL) { tracker->restarts = 0U; tracker->window_started_ns = now_ns; } }
