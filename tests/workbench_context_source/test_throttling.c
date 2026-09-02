/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_throttling.c
 *
 * PURPOSE:
 *   Verify per-source minimum intervals prevent excessive caret publication.
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
    UmiWorkbenchContextSourceSample sample;
    const UmiWorkbenchContextSourceRuntime *runtime;

    umi_workbench_context_source_definition_init(
        &definition, "test.live.editor");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.test",
        "test.source", "Editor") == UMI_STATUS_OK);
    definition.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR;
    definition.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)UMI_CONTEXT_KIND_SOURCE_LOCATION - 1U);
    definition.minimum_interval_ms = 25U;
    assert(umi_workbench_context_source_service_register(
        sources, &definition) == UMI_STATUS_OK);

    umi_workbench_context_source_sample_init(
        &sample, UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION, "sample");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "test.live.editor", "org.umicom.test",
        "test.source", "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_location(
        &sample, "C:/dev/main.c", "", 12U, 4U, 0U) == UMI_STATUS_OK);
    sample.timestamp_ms = 100U;
    assert(umi_workbench_context_source_service_submit(
        sources, &sample) == UMI_STATUS_OK);

    sample.line = 13U;
    sample.timestamp_ms = 110U;
    assert(umi_workbench_context_source_service_submit(
        sources, &sample) == UMI_STATUS_OK);
    runtime = umi_workbench_context_source_service_runtime(
        sources, "test.live.editor");
    assert(runtime != NULL);
    assert(runtime->accepted_count == 1U);
    assert(runtime->throttled_count == 1U);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
