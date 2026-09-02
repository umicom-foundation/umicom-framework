/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/circuit_breaker.h
 *
 * PURPOSE:
 *   Define a deterministic circuit breaker that limits repeated calls to failing
 *   providers and services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_CIRCUIT_BREAKER_H
#define UMICOM_RESILIENCE_CIRCUIT_BREAKER_H

#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named circuit state values accepted by this public contract.
 */
typedef enum UmiCircuitState { UMI_CIRCUIT_CLOSED = 0, UMI_CIRCUIT_OPEN = 1, UMI_CIRCUIT_HALF_OPEN = 2 } UmiCircuitState;
/**
 * Represent the circuit breaker config data shared with callers of this public contract.
 */
typedef struct UmiCircuitBreakerConfig { uint32_t failure_threshold; uint32_t success_threshold; uint64_t reset_timeout_ns; } UmiCircuitBreakerConfig;
/**
 * Represent the circuit breaker snapshot data shared with callers of this public contract.
 */
typedef struct UmiCircuitBreakerSnapshot { UmiCircuitState state; uint32_t consecutive_failures; uint32_t half_open_successes; uint64_t opened_at_ns; } UmiCircuitBreakerSnapshot;
/**
 * Represent the circuit breaker data shared with callers of this public contract.
 */
typedef struct UmiCircuitBreaker UmiCircuitBreaker;
/**
 * Provide the circuit breaker config default operation used by this module and its client
 * applications.
 */
UmiCircuitBreakerConfig umi_circuit_breaker_config_default(void);
/**
 * Initialise circuit breaker from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_circuit_breaker_create(const UmiCircuitBreakerConfig *config,
                                     UmiCircuitBreaker **out_breaker);
/**
 * Release or reset state held by circuit breaker so the same storage can be reused safely.
 */
void umi_circuit_breaker_destroy(UmiCircuitBreaker *breaker);
/**
 * Provide the circuit breaker before request operation used by this module and its client
 * applications.
 */
UmiStatus umi_circuit_breaker_before_request(UmiCircuitBreaker *breaker,
                                             uint64_t now_ns);
/**
 * Provide the circuit breaker record success operation used by this module and its client
 * applications.
 */
void umi_circuit_breaker_record_success(UmiCircuitBreaker *breaker);
/**
 * Provide the circuit breaker record failure operation used by this module and its client
 * applications.
 */
void umi_circuit_breaker_record_failure(UmiCircuitBreaker *breaker,
                                        uint64_t now_ns);
/**
 * Provide the circuit breaker snapshot operation used by this module and its client
 * applications.
 */
UmiCircuitBreakerSnapshot umi_circuit_breaker_snapshot(UmiCircuitBreaker *breaker);
#ifdef __cplusplus
}
#endif

#endif
