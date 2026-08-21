/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_service_controller.c
 *
 * PURPOSE:
 *   Verify the authoritative layout service and Layout Slave Controller
 *   lifecycle used by the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)calloc(1U, sizeof(*store));
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutService *service = NULL;
    UmiWorkbenchLayoutSlaveController *controller = NULL;
    UmiWorkbenchLayoutPrincipal principal = test_layout_principal();
    UmiWorkbenchLayoutCommand command;
    UmiWorkbenchLayoutCommandResult result;
    UmiWorkbenchLayoutControllerSnapshot snapshot;
    const UmiWorkbenchLayoutDocument *active;

    TEST_REQUIRE(store != NULL, "Memory store must be allocated");
    umi_workbench_memory_store_init(store);
    adapter = umi_workbench_memory_store_adapter(store);
    TEST_STATUS_OK(umi_workbench_layout_service_create(
        NULL, &adapter, &service));
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_create(
        NULL, service, &controller));
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_initialise(controller));
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_start(controller));

    umi_workbench_layout_command_init(
        &command,
        UMI_WORKBENCH_LAYOUT_COMMAND_CLONE,
        "command.create-development",
        "developer",
        "correlation.controller");
    test_copy_text(
        command.template_id,
        sizeof(command.template_id),
        "framework.development");
    test_copy_text(
        command.layout_id,
        sizeof(command.layout_id),
        "layout.controller");
    test_copy_text(
        command.name,
        sizeof(command.name),
        "Controller Development Layout");
    command.timestamp_ms = 5000U;
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_handle(
        controller, &principal, &command, &result));
    TEST_REQUIRE(result.changed, "Clone command must change service state");

    active = umi_workbench_layout_service_active_layout(service);
    TEST_REQUIRE(active != NULL, "Controller must establish an active layout");
    TEST_REQUIRE(
        strcmp(active->identity.layout_id, "layout.controller") == 0,
        "Active layout identity must match command");

    umi_workbench_layout_command_init(
        &command,
        UMI_WORKBENCH_LAYOUT_COMMAND_APPLY_OPERATION,
        "command.rename",
        "developer",
        "correlation.controller");
    test_copy_text(
        command.layout_id,
        sizeof(command.layout_id),
        "layout.controller");
    umi_workbench_layout_operation_init(
        &command.operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT,
        "operation.rename");
    test_copy_text(
        command.operation.text_value,
        sizeof(command.operation.text_value),
        "Controller Layout Revised");
    command.operation.expected_revision = active->version.revision;
    command.timestamp_ms = 5010U;
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_handle(
        controller, &principal, &command, &result));
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_service_active_layout(service)->name,
            "Controller Layout Revised") == 0,
        "Controller must delegate operation to service");

    umi_workbench_layout_command_init(
        &command,
        UMI_WORKBENCH_LAYOUT_COMMAND_SAVE,
        "command.save",
        "developer",
        "correlation.controller");
    test_copy_text(
        command.layout_id,
        sizeof(command.layout_id),
        "layout.controller");
    command.timestamp_ms = 5020U;
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_handle(
        controller, &principal, &command, &result));
    TEST_REQUIRE(
        umi_workbench_memory_store_layout_count(store) == 1U,
        "Save command must persist through Framework store adapter");

    TEST_STATUS_OK(umi_workbench_layout_slave_controller_snapshot(
        controller, &snapshot));
    TEST_REQUIRE(
        snapshot.state == UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING,
        "Controller snapshot must report running state");
    TEST_REQUIRE(
        snapshot.accepted_command_count == 3U,
        "Controller must count accepted commands");

    TEST_STATUS_OK(umi_workbench_layout_slave_controller_quiesce(controller));
    TEST_REQUIRE(
        umi_workbench_layout_slave_controller_handle(
            controller, &principal, &command, &result) == UMI_STATUS_BUSY,
        "Quiesced controller must reject new commands");
    TEST_STATUS_OK(umi_workbench_layout_slave_controller_stop(
        controller, 5100U));

    umi_workbench_layout_slave_controller_destroy(controller);
    umi_workbench_layout_service_destroy(service);
    umi_workbench_memory_store_clear(store);
    free(store);
    return 0;
}
