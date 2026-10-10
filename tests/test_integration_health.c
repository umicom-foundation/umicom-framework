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

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/health.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationLaunchPlan p = {0}; UmiIntegrationHealthSummary h;
    p.count=1U;
    p.items[0].disposition=UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING;
    p.missing_optional=1U;
    umi_integration_health_from_plan(&p,&h);
    CHECK(h.degraded && !h.healthy && h.missing_required==0U);

    /* The unmodified plan and extended suite observation states must both
     * project cleanly without changing the public summary structure. */
    p.missing_optional = 0U;
    p.items[0].kind = UMI_INTEGRATION_DEPENDENCY_REQUIRED;
    p.items[0].disposition = UMI_INTEGRATION_LAUNCH_READY;
    umi_integration_health_from_plan(&p, &h);
    CHECK(h.available == 1U && h.running == 0U && h.healthy);
    p.items[0].disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING;
    umi_integration_health_from_plan(&p, &h);
    CHECK(h.available == 1U && h.running == 1U && h.healthy);
    p.items[0].disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED;
    umi_integration_health_from_plan(&p, &h);
    CHECK(h.available == 0U && h.running == 0U && !h.healthy && !h.degraded);
    p.items[0].kind = UMI_INTEGRATION_DEPENDENCY_OPTIONAL;
    umi_integration_health_from_plan(&p, &h);
    CHECK(!h.healthy && h.degraded && h.missing_optional == 0U);
    /* R02: A successful launch plan does NOT prove suite runtime readiness.
     * A stopped/starting member retains its record but revokes usability. */
    {
        UmiIntegrationSuiteRuntime runtime = {0};
        runtime.plan.count = 1U;
        runtime.plan.items[0].kind = UMI_INTEGRATION_DEPENDENCY_REQUIRED;
        runtime.plan.items[0].disposition = UMI_INTEGRATION_LAUNCH_READY;
        runtime.state = UMI_INTEGRATION_SUITE_PREPARED;
        umi_integration_health_from_runtime(&runtime, &h);
        CHECK(!h.healthy && !h.degraded && h.available == 1U && h.running == 0U);
        runtime.plan.items[0].disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING;
        runtime.state = UMI_INTEGRATION_SUITE_STARTING;
        umi_integration_health_from_runtime(&runtime, &h);
        CHECK(!h.healthy && !h.degraded);
        runtime.plan.items[0].disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING;
        runtime.running_required = 1U;
        runtime.state = UMI_INTEGRATION_SUITE_RUNNING;
        umi_integration_health_from_runtime(&runtime, &h);
        CHECK(h.healthy && !h.degraded && h.running == 1U);
        runtime.plan.items[0].disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED;
        runtime.running_required = 0U;
        runtime.state = UMI_INTEGRATION_SUITE_STOPPED;
        umi_integration_health_from_runtime(&runtime, &h);
        CHECK(!h.healthy && !h.degraded && h.running == 0U);
        /* Malformed records have no publishable health claim. */
        runtime.plan.count = UMI_INTEGRATION_MAX_MEMBERS + 1U;
        umi_integration_health_from_runtime(&runtime, &h);
        CHECK(!h.healthy && !h.degraded && h.running == 0U);
        umi_integration_health_from_runtime(NULL, &h);
        CHECK(!h.healthy && !h.degraded && h.running == 0U);
        umi_integration_health_from_runtime(&runtime, NULL);
    }

    p.count = UMI_INTEGRATION_MAX_MEMBERS + 1U;
    umi_integration_health_from_plan(&p, &h);
    CHECK(h.available == 0U && !h.healthy);
    return 0;
}
