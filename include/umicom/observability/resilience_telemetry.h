/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/resilience_telemetry.h
 *
 * PURPOSE:
 *   Publish the public resilience telemetry contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_RESILIENCE_TELEMETRY_H
#define UMICOM_OBSERVABILITY_RESILIENCE_TELEMETRY_H
#include "umicom/observability/operations_contract.h"
#include "umicom/resilience/circuit_breaker.h"
#include "umicom/resilience/rate_limiter.h"
#include "umicom/resilience/supervisor.h"
typedef struct UmiOperationsResilienceTelemetry { size_t supervised_components; size_t running_components; size_t degraded_components; size_t failed_components; size_t total_restarts; UmiCircuitState circuit_state; uint64_t accepted_requests; uint64_t rejected_requests; } UmiOperationsResilienceTelemetry;
UmiStatus umi_operations_resilience_capture(UmiResilienceSupervisor *supervisor,UmiCircuitBreaker *circuit,UmiRateLimiter *limiter,UmiOperationsResilienceTelemetry *out_telemetry);
#endif
