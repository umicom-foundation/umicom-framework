/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/circuit_breaker.h
 *
 * PURPOSE:
 *   Define a deterministic circuit breaker that limits repeated calls to failing
 *   providers and services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_CIRCUIT_BREAKER_H
#define UMICOM_RESILIENCE_CIRCUIT_BREAKER_H

#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCircuitState { UMI_CIRCUIT_CLOSED = 0, UMI_CIRCUIT_OPEN = 1, UMI_CIRCUIT_HALF_OPEN = 2 } UmiCircuitState;
typedef struct UmiCircuitBreakerConfig { uint32_t failure_threshold; uint32_t success_threshold; uint64_t reset_timeout_ns; } UmiCircuitBreakerConfig;
typedef struct UmiCircuitBreakerSnapshot { UmiCircuitState state; uint32_t consecutive_failures; uint32_t half_open_successes; uint64_t opened_at_ns; } UmiCircuitBreakerSnapshot;
typedef struct UmiCircuitBreaker UmiCircuitBreaker;
UmiCircuitBreakerConfig umi_circuit_breaker_config_default(void);
UmiStatus umi_circuit_breaker_create(const UmiCircuitBreakerConfig *config,
                                     UmiCircuitBreaker **out_breaker);
void umi_circuit_breaker_destroy(UmiCircuitBreaker *breaker);
UmiStatus umi_circuit_breaker_before_request(UmiCircuitBreaker *breaker,
                                             uint64_t now_ns);
void umi_circuit_breaker_record_success(UmiCircuitBreaker *breaker);
void umi_circuit_breaker_record_failure(UmiCircuitBreaker *breaker,
                                        uint64_t now_ns);
UmiCircuitBreakerSnapshot umi_circuit_breaker_snapshot(UmiCircuitBreaker *breaker);
#ifdef __cplusplus
}
#endif

#endif
