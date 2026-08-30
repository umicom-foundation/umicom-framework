/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/resilience_telemetry.c
 *
 * PURPOSE:
 *   Implement the resilience telemetry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Resilience telemetry | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/resilience_telemetry.h"
#include <string.h>

UmiStatus umi_operations_resilience_capture(UmiResilienceSupervisor *supervisor,UmiCircuitBreaker *circuit,UmiRateLimiter *limiter,UmiOperationsResilienceTelemetry *out_telemetry)
{
    size_t index;
    UmiCircuitBreakerSnapshot circuit_snapshot;
    UmiRateLimiterSnapshot limiter_snapshot;
    if (supervisor == NULL || circuit == NULL || limiter == NULL || out_telemetry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_telemetry,0,sizeof(*out_telemetry));
    out_telemetry->supervised_components = umi_resilience_supervisor_count(supervisor);
    for (index = 0U; index < out_telemetry->supervised_components; ++index) {
        UmiSupervisedSnapshot item;
        if (umi_resilience_supervisor_at(supervisor,index,&item) != UMI_STATUS_OK) continue;
        out_telemetry->total_restarts += item.restarts;
        if (item.state == UMI_SUPERVISED_RUNNING) out_telemetry->running_components += 1U;
        else if (item.state == UMI_SUPERVISED_DEGRADED) out_telemetry->degraded_components += 1U;
        else if (item.state == UMI_SUPERVISED_FAILED) out_telemetry->failed_components += 1U;
    }
    circuit_snapshot = umi_circuit_breaker_snapshot(circuit);
    limiter_snapshot = umi_rate_limiter_snapshot(limiter);
    out_telemetry->circuit_state = circuit_snapshot.state;
    out_telemetry->accepted_requests = limiter_snapshot.accepted;
    out_telemetry->rejected_requests = limiter_snapshot.rejected;
    return UMI_STATUS_OK;
}
