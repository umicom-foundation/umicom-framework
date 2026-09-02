/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operations_resilience.c
 *
 * PURPOSE:
 *   Implement the test operations resilience behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations resilience tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/observability/resilience_telemetry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiResilienceSupervisor *supervisor = NULL;
    UmiCircuitBreaker *circuit = NULL;
    UmiRateLimiter *limiter = NULL;
    UmiCircuitBreakerConfig circuit_config = {2U,1U,100U};
    UmiRateLimiterConfig limiter_config = {1.0,0.0};
    UmiOperationsResilienceTelemetry telemetry;
    UmiRestartDecision decision;
    assert(umi_resilience_supervisor_create(&supervisor) == UMI_STATUS_OK);
    assert(umi_resilience_supervisor_register(supervisor,"indexer",NULL,0U) == UMI_STATUS_OK);
    assert(umi_resilience_supervisor_failure(supervisor,"indexer",UMI_STATUS_UNAVAILABLE,10U,&decision) == UMI_STATUS_OK);
    assert(umi_circuit_breaker_create(&circuit_config,&circuit) == UMI_STATUS_OK);
    assert(umi_rate_limiter_create(&limiter_config,0U,&limiter) == UMI_STATUS_OK);
    assert(umi_rate_limiter_allow(limiter,1.0,0U));
    assert(!umi_rate_limiter_allow(limiter,1.0,0U));
    assert(umi_operations_resilience_capture(supervisor,circuit,limiter,&telemetry) == UMI_STATUS_OK);
    assert(telemetry.supervised_components == 1U);
    assert(telemetry.degraded_components == 1U && telemetry.total_restarts == 1U);
    assert(telemetry.accepted_requests == 1U && telemetry.rejected_requests == 1U);
    umi_rate_limiter_destroy(limiter); umi_circuit_breaker_destroy(circuit); umi_resilience_supervisor_destroy(supervisor);
    return 0;
}
