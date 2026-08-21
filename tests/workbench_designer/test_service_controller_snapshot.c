/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_service_controller_snapshot.c
 *
 * PURPOSE:
 *   Verify Layout Designer service sessions, Slave Controller lifecycle and
 *   immutable snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerService *service;
    UmiWorkbenchDesignerSession *session;
    UmiWorkbenchDesignerController controller;
    UmiWorkbenchDesignerSnapshot snapshot;
    UmiWorkbenchDesignerControllerCommand command;
    UmiWorkbenchDesignerControllerResult result;

    TEST_REQUIRE_STATUS(test_make_controller(
        &service, &controller, &session));
    TEST_REQUIRE(service != NULL && session != NULL);
    TEST_REQUIRE(controller.state == UMI_WORKBENCH_DESIGNER_STATE_RUNNING);
    TEST_REQUIRE_STATUS(umi_workbench_designer_snapshot_capture(
        &controller, &snapshot));
    TEST_REQUIRE(snapshot.session_count == 1U);
    TEST_REQUIRE(strcmp(snapshot.active_session_id, "session.fixture") == 0);

    umi_workbench_designer_controller_command_init(
        &command, UMI_WORKBENCH_DESIGNER_CONTROLLER_SET_MODE,
        "controller.preview");
    TEST_REQUIRE_STATUS(test_copy_text(
        command.session_id, sizeof(command.session_id), "session.fixture"));
    command.mode = UMI_WORKBENCH_DESIGNER_MODE_PREVIEW;
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_dispatch(
        &controller, &command, &result));
    TEST_REQUIRE(result.status == UMI_STATUS_OK);
    TEST_REQUIRE(umi_workbench_designer_session_mode(session) ==
        UMI_WORKBENCH_DESIGNER_MODE_PREVIEW);
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_quiesce(&controller));
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_stop(&controller));
    umi_workbench_designer_service_destroy(service);
    return 0;
}
