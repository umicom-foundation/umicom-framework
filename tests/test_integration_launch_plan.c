/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_launch_plan.c
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
    UmiIntegrationRegistry r; UmiIntegrationApplication a;
    UmiIntegrationSuiteDefinition s; UmiIntegrationLaunchPlan p;
    umi_integration_registry_init(&r); umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"studio","Studio")==UMI_STATUS_OK);
    a.enabled=true;
    CHECK(umi_integration_registry_register(&r,&a)==UMI_STATUS_OK);
    umi_integration_suite_init(&s,"suite","Suite");
    CHECK(umi_integration_suite_add_member(&s,"studio",UMI_INTEGRATION_DEPENDENCY_REQUIRED,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_add_member(&s,"future",UMI_INTEGRATION_DEPENDENCY_OPTIONAL,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_launch_plan_build(&s,&r,&p)==UMI_STATUS_OK);
    CHECK(umi_integration_launch_plan_can_start(&p));
    CHECK(p.missing_optional==1U);
    return 0;
}
