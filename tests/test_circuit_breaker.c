/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_circuit_breaker.c
 *
 * PURPOSE:
 *   Verify circuit opening, reset timeout and half-open recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/resilience/circuit_breaker.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiCircuitBreakerConfig c = {2U,1U,100U}; UmiCircuitBreaker *b = NULL;
    assert(umi_circuit_breaker_create(&c, &b) == UMI_STATUS_OK);
    umi_circuit_breaker_record_failure(b, 10U); umi_circuit_breaker_record_failure(b, 20U);
    assert(umi_circuit_breaker_before_request(b, 30U) == UMI_STATUS_UNAVAILABLE);
    assert(umi_circuit_breaker_before_request(b, 120U) == UMI_STATUS_OK);
    umi_circuit_breaker_record_success(b);
    assert(umi_circuit_breaker_snapshot(b).state == UMI_CIRCUIT_CLOSED);
    umi_circuit_breaker_destroy(b); return 0;
}
