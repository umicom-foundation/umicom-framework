/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_health.c
 *
 * PURPOSE:
 *   Verify source health transitions from healthy to degraded when suspended.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "test_support.h"
#include "umicom/workbench_context_source/workbench_context_source.h"


int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextEventService *events = NULL;
    UmiWorkbenchContextSourceService *sources = NULL;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_event_service_create(
        fixture.host, &events) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_service_create(
        events, &sources) == UMI_STATUS_OK);

    UmiWorkbenchContextSourceDefinition definition;
    UmiWorkbenchContextSourceHealth health;
    umi_workbench_context_source_definition_init(
        &definition, "test.source.definition");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.test", "test.source",
        "Source") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_service_register(
        sources, &definition) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_health_evaluate(
        sources, &health) == UMI_STATUS_OK);
    assert(health.state ==
        UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_HEALTHY);
    umi_workbench_context_source_service_set_suspended(
        sources, true);
    assert(umi_workbench_context_source_health_evaluate(
        sources, &health) == UMI_STATUS_OK);
    assert(health.state ==
        UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_DEGRADED);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
