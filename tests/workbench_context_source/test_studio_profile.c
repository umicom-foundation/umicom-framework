/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_studio_profile.c
 *
 * PURPOSE:
 *   Verify the reusable Studio live-source profile registers editor, project, Problems, VCS, test, debug and AI sources.
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

    UmiWorkbenchContextSourceStudioProfile profile =
        umi_workbench_context_source_studio_profile_default();
    assert(umi_workbench_context_source_register_studio_profile(
        sources, &profile) == UMI_STATUS_OK);
    assert(sources->registry.count == 7U);
    assert(umi_workbench_context_source_service_find(
        sources, "studio.editor.location") != NULL);
    assert(umi_workbench_context_source_service_find(
        sources, "studio.source-control.selection") != NULL);
    assert(umi_workbench_context_source_service_find(
        sources, "studio.ai.selection") != NULL);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
