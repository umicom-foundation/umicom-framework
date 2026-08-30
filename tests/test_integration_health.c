/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_health.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/health.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationLaunchPlan p = {0}; UmiIntegrationHealthSummary h;
    p.count=1U;
    p.items[0].disposition=UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING;
    p.missing_optional=1U;
    umi_integration_health_from_plan(&p,&h);
    CHECK(h.degraded && !h.healthy && h.missing_required==0U);
    return 0;
}
