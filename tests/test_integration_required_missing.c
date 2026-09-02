/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_required_missing.c
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

#include "umicom/integration/launch_plan.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationRegistry r; UmiIntegrationSuiteDefinition s; UmiIntegrationLaunchPlan p;
    umi_integration_registry_init(&r); umi_integration_suite_init(&s,"suite","Suite");
    CHECK(umi_integration_suite_add_member(&s,"must.exist",UMI_INTEGRATION_DEPENDENCY_REQUIRED,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_launch_plan_build(&s,&r,&p)==UMI_STATUS_UNAVAILABLE);
    CHECK(!umi_integration_launch_plan_can_start(&p));
    return 0;
}
