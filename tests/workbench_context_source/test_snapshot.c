/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_snapshot.c
 *
 * PURPOSE:
 *   Verify live-source snapshot reports configured sources and event queue state.
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


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    UmiWorkbenchContextSourceSnapshot snapshot;
    umi_workbench_context_source_definition_init(
        &definition, "test.source.definition");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.test", "test.source",
        "Source") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_service_register(
        sources, &definition) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_snapshot_build(
        sources, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.source_count == 1U);
    assert(snapshot.active_source_count == 1U);
    assert(snapshot.runtime_count == 1U);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
