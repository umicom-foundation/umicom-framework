/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_suite_runtime.c
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

#include "umicom/integration/suite_runtime.h"
#include "umicom/integration/health.h"

#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationRegistry r; UmiIntegrationApplication a;
    UmiIntegrationSuiteDefinition s; UmiIntegrationSuiteRuntime rt;
    UmiIntegrationSuiteRuntime unchanged;
    UmiIntegrationHealthSummary h;
    umi_integration_registry_init(&r); umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"core","Core")==UMI_STATUS_OK);
    a.enabled=true;
    CHECK(umi_integration_registry_register(&r,&a)==UMI_STATUS_OK);
    umi_integration_suite_init(&s,"suite","Suite");
    CHECK(umi_integration_suite_add_member(&s,"core",UMI_INTEGRATION_DEPENDENCY_REQUIRED,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_add_member(&s,"optional",UMI_INTEGRATION_DEPENDENCY_OPTIONAL,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_runtime_prepare(&rt,&s,&r)==UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_PREPARED);
    CHECK(!umi_integration_suite_runtime_is_usable(&rt));
    CHECK(umi_integration_suite_runtime_mark_running(&rt,"core")==UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_DEGRADED);
    CHECK(rt.running_required == 1U);
    CHECK(umi_integration_suite_runtime_is_usable(&rt));

    /* A second identical callback cannot invent another running process. */
    unchanged = rt;
    CHECK(umi_integration_suite_runtime_mark_running(&rt, "core") == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&rt, &unchanged, sizeof(rt)) == 0);
    CHECK(umi_integration_suite_runtime_mark_running(&rt, "optional") == UMI_STATUS_INVALID_STATE);
    CHECK(umi_integration_suite_runtime_mark_failed(&rt, "optional") == UMI_STATUS_INVALID_STATE);

    /* A new REQUIRED member must run before the suite becomes usable. */
    umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"worker","Worker")==UMI_STATUS_OK);
    a.enabled = true;
    CHECK(umi_integration_registry_register(&r, &a) == UMI_STATUS_OK);
    CHECK(umi_integration_suite_add_member(&s,"worker",UMI_INTEGRATION_DEPENDENCY_REQUIRED,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_runtime_prepare(&rt, &s, &r) == UMI_STATUS_OK);
    CHECK(!umi_integration_suite_runtime_is_usable(&rt));
    CHECK(umi_integration_suite_runtime_mark_running(&rt, "core") == UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_STARTING && !umi_integration_suite_runtime_is_usable(&rt));
    CHECK(umi_integration_suite_runtime_mark_running(&rt, "core") == UMI_STATUS_INVALID_STATE);
    CHECK(rt.running_required == 1U);
    CHECK(umi_integration_suite_runtime_mark_running(&rt, "worker") == UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_DEGRADED && rt.running_required == 2U);
    CHECK(umi_integration_suite_runtime_is_usable(&rt));

    /* Observed failure removes running evidence for exactly that member. */
    CHECK(umi_integration_suite_runtime_mark_failed(&rt,"core") == UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_FAILED && rt.failed_required == 1U);
    CHECK(rt.running_required == 1U && !umi_integration_suite_runtime_is_usable(&rt));
    unchanged = rt;
    CHECK(umi_integration_suite_runtime_mark_failed(&rt,"core") == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&rt, &unchanged, sizeof(rt)) == 0);
    umi_integration_health_from_plan(&rt.plan, &h);
    CHECK(!h.healthy && !h.degraded && h.running == 1U);
    CHECK(umi_integration_suite_runtime_mark_running(&rt,"core") == UMI_STATUS_OK);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_DEGRADED && rt.failed_required == 0U);
    CHECK(rt.running_required == 2U && umi_integration_suite_runtime_is_usable(&rt));
    umi_integration_health_from_plan(&rt.plan, &h);
    CHECK(h.running == 2U && h.degraded);

    /* Existing running processes are counted only once at prepare time. */
    CHECK(umi_integration_registry_set_state(&r,"core",UMI_INTEGRATION_APP_RUNNING)==UMI_STATUS_OK);
    CHECK(umi_integration_registry_set_state(&r,"worker",UMI_INTEGRATION_APP_RUNNING)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_runtime_prepare(&rt,&s,&r)==UMI_STATUS_OK);
    CHECK(rt.running_required == 2U && rt.state == UMI_INTEGRATION_SUITE_DEGRADED);
    CHECK(umi_integration_suite_runtime_is_usable(&rt));
    CHECK(umi_integration_suite_runtime_mark_running(&rt,"core") == UMI_STATUS_INVALID_STATE);
    CHECK(rt.running_required == 2U);

    /* A PRESENT optional member may fail and later recover without
     * duplicating its failed or running evidence. This test has no missing
     * optional entries, so successful recovery really becomes RUNNING. */
    umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"media","Media")==UMI_STATUS_OK);
    a.enabled = true;
    CHECK(umi_integration_registry_register(&r, &a) == UMI_STATUS_OK);
    umi_integration_suite_init(&s,"creative","Creative suite");
    CHECK(umi_integration_suite_add_member(&s,"core",UMI_INTEGRATION_DEPENDENCY_REQUIRED,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_add_member(&s,"media",UMI_INTEGRATION_DEPENDENCY_OPTIONAL,0U)==UMI_STATUS_OK);
    CHECK(umi_integration_suite_runtime_prepare(&rt,&s,&r)==UMI_STATUS_OK);
    CHECK(rt.running_required == 1U && rt.state == UMI_INTEGRATION_SUITE_RUNNING);
    CHECK(umi_integration_suite_runtime_mark_failed(&rt,"media") == UMI_STATUS_OK);
    CHECK(rt.failed_optional == 1U && rt.state == UMI_INTEGRATION_SUITE_DEGRADED);
    CHECK(umi_integration_suite_runtime_is_usable(&rt));
    CHECK(umi_integration_suite_runtime_mark_failed(&rt,"media") == UMI_STATUS_INVALID_STATE);
    CHECK(rt.failed_optional == 1U);
    CHECK(umi_integration_suite_runtime_mark_running(&rt,"media") == UMI_STATUS_OK);
    CHECK(rt.failed_optional == 0U && rt.running_optional == 1U);
    CHECK(rt.state == UMI_INTEGRATION_SUITE_RUNNING && umi_integration_suite_runtime_is_usable(&rt));
    umi_integration_health_from_plan(&rt.plan, &h);
    CHECK(h.healthy && !h.degraded && h.running == 2U);

    /* Malformed suite input must not corrupt a live runtime snapshot. */
    unchanged = rt;
    s.member_count = UMI_INTEGRATION_MAX_MEMBERS + 1U;
    CHECK(umi_integration_suite_runtime_prepare(&rt,&s,&r) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&rt,&unchanged,sizeof(rt)) == 0);
    return 0;
}
