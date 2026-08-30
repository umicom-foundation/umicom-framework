/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_dynamic_group.c
 *
 * PURPOSE:
 *   Verify events without group overrides follow the panel's current Context Host assignment.
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
    UmiWorkbenchContextSourceSample sample;
    const UmiWorkbenchContextLinkActiveSlot *active;

    assert(umi_workbench_context_host_assign_endpoint_group(
        fixture.host,
        "test.source.endpoint",
        "red",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);

    umi_workbench_context_source_definition_init(
        &definition, "test.dynamic.selection");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.test",
        "test.source", "Dynamic Source") == UMI_STATUS_OK);
    definition.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC;
    definition.context_kind = UMI_CONTEXT_KIND_SELECTION;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)UMI_CONTEXT_KIND_SELECTION - 1U);
    definition.preferred_group_id[0] = '\0';
    assert(umi_workbench_context_source_service_register(
        sources, &definition) == UMI_STATUS_OK);

    umi_workbench_context_source_sample_init(
        &sample, UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_SELECTION, "dynamic");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "test.dynamic.selection", "org.umicom.test",
        "test.source", NULL) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_subject(
        &sample, "item", "item") == UMI_STATUS_OK);
    sample.timestamp_ms = 100U;

    assert(umi_workbench_context_source_service_submit(
        sources, &sample) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "red");
    assert(active != NULL);
    assert(strcmp(active->payload.identity.context_id, "dynamic") == 0);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
