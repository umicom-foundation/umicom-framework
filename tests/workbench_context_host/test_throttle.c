/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_throttle.c
 *
 * PURPOSE:
 *   Verify high-frequency observations are throttled by monotonic interval.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/throttle.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostThrottle throttle;
    umi_workbench_context_host_throttle_init(&throttle, 50U);
    assert(umi_workbench_context_host_throttle_accept(&throttle, 100U));
    assert(!umi_workbench_context_host_throttle_accept(&throttle, 120U));
    assert(umi_workbench_context_host_throttle_accept(&throttle, 150U));
    assert(throttle.accepted_count == 2U);
    assert(throttle.throttled_count == 1U);

    return 0;
}
