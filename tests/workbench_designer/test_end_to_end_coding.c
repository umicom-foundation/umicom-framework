/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_end_to_end_coding.c
 *
 * PURPOSE:
 *   Exercise coding-layout creation, session editing, selection, save gating,
 *   snapshot and close lifecycle.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchDesignerLayoutFactoryRequest request =
        umi_workbench_designer_layout_factory_request_default();
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerService *service;
    UmiWorkbenchDesignerSession *session;
    UmiWorkbenchDesignerController controller;
    UmiWorkbenchDesignerCommand command;
    UmiWorkbenchDesignerCommandResult command_result;
    UmiWorkbenchDesignerSnapshot snapshot;

    request.profile = UMI_WORKBENCH_DESIGNER_LAYOUT_CODING;
    TEST_REQUIRE_STATUS(test_copy_text(
        request.layout_id, sizeof(request.layout_id), "layout.end-to-end.coding"));
    TEST_REQUIRE_STATUS(test_copy_text(
        request.name, sizeof(request.name), "End-to-End Coding"));
    umi_workbench_designer_palette_init(&palette);
    TEST_REQUIRE_STATUS(umi_workbench_designer_palette_seed_framework(&palette));
    TEST_REQUIRE_STATUS(umi_workbench_designer_layout_factory_create(
        &request, &palette, &document));
    TEST_REQUIRE_STATUS(umi_workbench_designer_service_create(NULL, &service));
    TEST_REQUIRE_STATUS(umi_workbench_designer_service_open(
        service, "session.coding", &document, NULL, &session));
    umi_workbench_designer_controller_init(
        &controller, "layout-designer", service);
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_initialise(&controller));
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_start(&controller));
    TEST_REQUIRE_STATUS(umi_workbench_designer_session_select(
        session, "editor", false, false));
    umi_workbench_designer_command_init(
        &command, UMI_WORKBENCH_DESIGNER_COMMAND_SET_PROPERTY,
        "command.coding.rename");
    TEST_REQUIRE_STATUS(test_copy_text(
        command.target_node_id, sizeof(command.target_node_id), "editor"));
    TEST_REQUIRE_STATUS(test_copy_text(
        command.component_id, sizeof(command.component_id), "title"));
    TEST_REQUIRE_STATUS(test_copy_text(
        command.text_value, sizeof(command.text_value), "C23 Editor"));
    command.expected_revision = document.version.revision;
    TEST_REQUIRE_STATUS(umi_workbench_designer_session_apply(
        session, &command, &command_result));
    TEST_REQUIRE(command_result.document_changed);
    TEST_REQUIRE(umi_workbench_designer_session_dirty(session));
    TEST_REQUIRE_STATUS(umi_workbench_designer_snapshot_capture(
        &controller, &snapshot));
    TEST_REQUIRE(snapshot.selection_count == 1U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_controller_stop(&controller));
    umi_workbench_designer_service_destroy(service);
    return 0;
}
