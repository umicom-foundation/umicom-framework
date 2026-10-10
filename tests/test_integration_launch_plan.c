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
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationRegistry r; UmiIntegrationApplication a;
    UmiIntegrationSuiteDefinition s; UmiIntegrationLaunchPlan p;
    UmiIntegrationLaunchPlan unchanged;
    UmiIntegrationSuiteDefinition invalid;
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

    /* A genuine missing required application publishes a complete plan. */
    CHECK(p.count == 2U && p.ready_count == 1U);
    CHECK(p.items[0].disposition == UMI_INTEGRATION_LAUNCH_READY);
    CHECK(p.items[1].disposition == UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING);
    s.members[1].kind = UMI_INTEGRATION_DEPENDENCY_REQUIRED;
    CHECK(umi_integration_launch_plan_build(&s, &r, &p) == UMI_STATUS_UNAVAILABLE);
    CHECK(p.count == 2U && p.missing_required == 1U && !umi_integration_launch_plan_can_start(&p));

    /* A malformed plan is never partly published. Capture byte-for-byte
     * evidence that the caller's previous result is still intact. */
    unchanged = p;
    invalid = s;
    invalid.member_count = UMI_INTEGRATION_MAX_MEMBERS + 1U;
    CHECK(umi_integration_launch_plan_build(&invalid, &r, &p) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);

    invalid = s;
    invalid.members[0].kind = (UmiIntegrationDependencyKind)99;
    CHECK(umi_integration_launch_plan_build(&invalid, &r, &p) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);

    invalid = s;
    (void)memset(invalid.members[0].application_id, 'x', sizeof(invalid.members[0].application_id));
    CHECK(umi_integration_launch_plan_build(&invalid, &r, &p) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);

    invalid = s;
    invalid.members[1] = invalid.members[0];
    CHECK(umi_integration_launch_plan_build(&invalid, &r, &p) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);

    invalid = s;
    (void)memset(invalid.id, 'x', sizeof(invalid.id));
    CHECK(umi_integration_launch_plan_build(&invalid, &r, &p) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);

    r.count = UMI_INTEGRATION_MAX_APPLICATIONS + 1U;
    CHECK(umi_integration_launch_plan_build(&s, &r, &p) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&p, &unchanged, sizeof(p)) == 0);
    r.count = 1U;

    /* An already-running member is a different observation from launchable. */
    CHECK(umi_integration_registry_set_state(&r, "studio", UMI_INTEGRATION_APP_RUNNING) == UMI_STATUS_OK);
    s.members[1].kind = UMI_INTEGRATION_DEPENDENCY_OPTIONAL;
    CHECK(umi_integration_launch_plan_build(&s, &r, &p) == UMI_STATUS_OK);
    CHECK(p.items[0].disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING);
    CHECK(p.items[1].disposition == UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING);
    return 0;
}
