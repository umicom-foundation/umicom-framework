/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/circuit_breaker.c
 *
 * PURPOSE:
 *   Implement closed, open and half-open transitions with configurable
 *   thresholds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/circuit_breaker.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiCircuitBreaker { UmiCircuitBreakerConfig config; UmiCircuitBreakerSnapshot state; UmiMutex *mutex; };
UmiCircuitBreakerConfig umi_circuit_breaker_config_default(void)
{ UmiCircuitBreakerConfig c = {5U, 1U, UINT64_C(30000000000)}; return c; }
UmiStatus umi_circuit_breaker_create(const UmiCircuitBreakerConfig *config, UmiCircuitBreaker **out_breaker)
{
    UmiCircuitBreaker *breaker; UmiStatus status;
    if (out_breaker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_breaker = NULL; breaker = (UmiCircuitBreaker *)calloc(1U, sizeof(*breaker)); if (breaker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    breaker->config = config != NULL ? *config : umi_circuit_breaker_config_default();
    if (breaker->config.failure_threshold == 0U || breaker->config.success_threshold == 0U) { free(breaker); return UMI_STATUS_INVALID_ARGUMENT; }
    status = umi_mutex_create(&breaker->mutex); if (status != UMI_STATUS_OK) { free(breaker); return status; }
    breaker->state.state = UMI_CIRCUIT_CLOSED; *out_breaker = breaker; return UMI_STATUS_OK;
}
void umi_circuit_breaker_destroy(UmiCircuitBreaker *breaker) { if (breaker != NULL) { umi_mutex_destroy(breaker->mutex); free(breaker); } }
UmiStatus umi_circuit_breaker_before_request(UmiCircuitBreaker *breaker, uint64_t now_ns)
{
    UmiStatus status;
    if (breaker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(breaker->mutex); if (status != UMI_STATUS_OK) return status;
    if (breaker->state.state == UMI_CIRCUIT_OPEN && now_ns - breaker->state.opened_at_ns >= breaker->config.reset_timeout_ns) { breaker->state.state = UMI_CIRCUIT_HALF_OPEN; breaker->state.half_open_successes = 0U; }
    status = breaker->state.state == UMI_CIRCUIT_OPEN ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    (void)umi_mutex_unlock(breaker->mutex); return status;
}
void umi_circuit_breaker_record_success(UmiCircuitBreaker *breaker)
{
    if (breaker == NULL || umi_mutex_lock(breaker->mutex) != UMI_STATUS_OK) return;
    breaker->state.consecutive_failures = 0U;
    if (breaker->state.state == UMI_CIRCUIT_HALF_OPEN && ++breaker->state.half_open_successes >= breaker->config.success_threshold) { breaker->state.state = UMI_CIRCUIT_CLOSED; breaker->state.half_open_successes = 0U; }
    (void)umi_mutex_unlock(breaker->mutex);
}
void umi_circuit_breaker_record_failure(UmiCircuitBreaker *breaker, uint64_t now_ns)
{
    if (breaker == NULL || umi_mutex_lock(breaker->mutex) != UMI_STATUS_OK) return;
    ++breaker->state.consecutive_failures;
    if (breaker->state.state == UMI_CIRCUIT_HALF_OPEN || breaker->state.consecutive_failures >= breaker->config.failure_threshold) { breaker->state.state = UMI_CIRCUIT_OPEN; breaker->state.opened_at_ns = now_ns; breaker->state.half_open_successes = 0U; }
    (void)umi_mutex_unlock(breaker->mutex);
}
UmiCircuitBreakerSnapshot umi_circuit_breaker_snapshot(UmiCircuitBreaker *breaker)
{ UmiCircuitBreakerSnapshot s = {0}; if (breaker != NULL && umi_mutex_lock(breaker->mutex) == UMI_STATUS_OK) { s = breaker->state; (void)umi_mutex_unlock(breaker->mutex); } return s; }
