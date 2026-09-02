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
/*
 * Provide the circuit breaker config default operation used by this module and its client
 * applications.
 */
UmiCircuitBreakerConfig umi_circuit_breaker_config_default(void)
{ UmiCircuitBreakerConfig c = {5U, 1U, UINT64_C(30000000000)}; return c; }
/*
 * Initialise circuit breaker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_circuit_breaker_create(const UmiCircuitBreakerConfig *config, UmiCircuitBreaker **out_breaker)
{
    UmiCircuitBreaker *breaker; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_breaker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_breaker = NULL; breaker = (UmiCircuitBreaker *)calloc(1U, sizeof(*breaker)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (breaker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    breaker->config = config != NULL ? *config : umi_circuit_breaker_config_default();
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (breaker->config.failure_threshold == 0U || breaker->config.success_threshold == 0U) { free(breaker); return UMI_STATUS_INVALID_ARGUMENT; }
    status = umi_mutex_create(&breaker->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(breaker); return status; }
    breaker->state.state = UMI_CIRCUIT_CLOSED; *out_breaker = breaker; return UMI_STATUS_OK;
}
/* Release or reset state held by circuit breaker so the same storage can be reused safely. */
void umi_circuit_breaker_destroy(UmiCircuitBreaker *breaker) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (breaker != NULL) { umi_mutex_destroy(breaker->mutex); free(breaker); } }
/*
 * Provide the circuit breaker before request operation used by this module and its client
 * applications.
 */
UmiStatus umi_circuit_breaker_before_request(UmiCircuitBreaker *breaker, uint64_t now_ns)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (breaker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(breaker->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (breaker->state.state == UMI_CIRCUIT_OPEN && now_ns - breaker->state.opened_at_ns >= breaker->config.reset_timeout_ns) { breaker->state.state = UMI_CIRCUIT_HALF_OPEN; breaker->state.half_open_successes = 0U; }
    status = breaker->state.state == UMI_CIRCUIT_OPEN ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    (void)umi_mutex_unlock(breaker->mutex); return status;
}
/*
 * Provide the circuit breaker record success operation used by this module and its client
 * applications.
 */
void umi_circuit_breaker_record_success(UmiCircuitBreaker *breaker)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (breaker == NULL || umi_mutex_lock(breaker->mutex) != UMI_STATUS_OK) return;
    breaker->state.consecutive_failures = 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (breaker->state.state == UMI_CIRCUIT_HALF_OPEN && ++breaker->state.half_open_successes >= breaker->config.success_threshold) { breaker->state.state = UMI_CIRCUIT_CLOSED; breaker->state.half_open_successes = 0U; }
    (void)umi_mutex_unlock(breaker->mutex);
}
/*
 * Provide the circuit breaker record failure operation used by this module and its client
 * applications.
 */
void umi_circuit_breaker_record_failure(UmiCircuitBreaker *breaker, uint64_t now_ns)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (breaker == NULL || umi_mutex_lock(breaker->mutex) != UMI_STATUS_OK) return;
    ++breaker->state.consecutive_failures;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (breaker->state.state == UMI_CIRCUIT_HALF_OPEN || breaker->state.consecutive_failures >= breaker->config.failure_threshold) { breaker->state.state = UMI_CIRCUIT_OPEN; breaker->state.opened_at_ns = now_ns; breaker->state.half_open_successes = 0U; }
    (void)umi_mutex_unlock(breaker->mutex);
}
/*
 * Provide the circuit breaker snapshot operation used by this module and its client
 * applications.
 */
UmiCircuitBreakerSnapshot umi_circuit_breaker_snapshot(UmiCircuitBreaker *breaker)
{ UmiCircuitBreakerSnapshot s = {0}; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (breaker != NULL && umi_mutex_lock(breaker->mutex) == UMI_STATUS_OK) { s = breaker->state; (void)umi_mutex_unlock(breaker->mutex); } return s; }
