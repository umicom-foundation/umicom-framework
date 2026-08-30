/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_rate_limiter.c
 *
 * PURPOSE:
 *   Verify token-bucket admission, rejection and refill.
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
#include "umicom/resilience/rate_limiter.h"
int main(void) {
    UmiRateLimiterConfig c = {2.0,1.0}; UmiRateLimiter *l = NULL;
    assert(umi_rate_limiter_create(&c, 0U, &l) == UMI_STATUS_OK);
    assert(umi_rate_limiter_allow(l,1.0,0U)); assert(umi_rate_limiter_allow(l,1.0,0U)); assert(!umi_rate_limiter_allow(l,1.0,0U));
    assert(umi_rate_limiter_allow(l,1.0,UINT64_C(1000000000)));
    umi_rate_limiter_destroy(l); return 0;
}
