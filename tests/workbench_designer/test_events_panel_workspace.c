/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_events_panel_workspace.c
 *
 * PURPOSE:
 *   Verify event publication, panel catalogue seeding and task-oriented
 *   workspace projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Exercise test event listener and return a clear result when the behaviour no longer
 * matches its contract.
 */
static void test_event_listener(
    const UmiWorkbenchDesignerEvent *event,
    void *context)
{
    size_t *count = context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event != NULL && count != NULL) *count += 1U;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchDesignerEventBus bus;
    UmiWorkbenchDesignerEvent event;
    UmiWorkbenchDesignerPanelCatalogue catalogue;
    UmiWorkbenchDesignerWorkspace workspace;
    size_t received = 0U;

    umi_workbench_designer_event_bus_init(&bus);
    TEST_REQUIRE_STATUS(umi_workbench_designer_event_subscribe(
        &bus, "test.listener", test_event_listener, &received));
    (void)memset(&event, 0, sizeof(event));
    event.kind = UMI_WORKBENCH_DESIGNER_EVENT_DOCUMENT_CHANGED;
    TEST_REQUIRE_STATUS(test_copy_text(
        event.event_id, sizeof(event.event_id), "event.document.changed"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_event_publish(&bus, &event));
    TEST_REQUIRE(received == 1U);
    TEST_REQUIRE(event.sequence == 1U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_event_unsubscribe(
        &bus, "test.listener"));

    umi_workbench_designer_panel_catalogue_init(&catalogue);
    TEST_REQUIRE_STATUS(umi_workbench_designer_panel_catalogue_seed(
        &catalogue));
    TEST_REQUIRE(catalogue.count >= 5U);
    umi_workbench_designer_workspace_init(&workspace);
    TEST_REQUIRE_STATUS(umi_workbench_designer_workspace_build(
        &workspace, &catalogue, UMI_WORKBENCH_DESIGNER_MODE_DESIGN,
        true, true));
    TEST_REQUIRE(workspace.count > 0U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_workspace_activate(
        &workspace, workspace.panels[0].panel_id));
    TEST_REQUIRE(workspace.active_panel_id[0] != '\0');
    return 0;
}
