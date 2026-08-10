/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_retry.c
 *
 * PURPOSE:
 *   Verify transient operations are retried and successful completion is
 *   reported.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/resilience/retry.h"
typedef struct Counter { int calls; } Counter;
static UmiStatus operation(void *data, uint32_t attempt) { Counter *c = (Counter *)data; ++c->calls; return attempt < 2U ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK; }
int main(void) {
    UmiResilienceRetryPolicy p = umi_resilience_retry_policy_default(); UmiResilienceRetryResult r; Counter c = {0};
    p.maximum_attempts = 4U; p.sleep_between_attempts = 0;
    assert(umi_resilience_retry_execute(&p, operation, &c, NULL, &r) == UMI_STATUS_OK);
    assert(r.attempts == 3U && c.calls == 3);
    return 0;
}
