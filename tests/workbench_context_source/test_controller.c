/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_controller.c
 *
 * PURPOSE:
 *   Verify the live interaction Source Slave Controller lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

    UmiWorkbenchContextSourceSlaveController controller;
    umi_workbench_context_source_slave_controller_init(
        &controller, sources);
    assert(umi_workbench_context_source_slave_controller_start(
        &controller) == UMI_STATUS_OK);
    assert(controller.state ==
        UMI_WORKBENCH_CONTEXT_SOURCE_CONTROLLER_RUNNING);
    assert(umi_workbench_context_source_slave_controller_quiesce(
        &controller) == UMI_STATUS_OK);
    assert(sources->suspended);
    assert(umi_workbench_context_source_slave_controller_resume(
        &controller) == UMI_STATUS_OK);
    assert(!sources->suspended);
    assert(umi_workbench_context_source_slave_controller_stop(
        &controller) == UMI_STATUS_OK);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
