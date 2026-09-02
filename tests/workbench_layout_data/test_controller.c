/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_controller.c
 *
 * PURPOSE:
 *   Verify Layout Persistence Slave Controller lifecycle and typed Master Controller command dispatch.
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
 * Exercise command base and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiWorkbenchLayoutDataCommand command_base(
    UmiWorkbenchLayoutDataCommandKind kind,
    const char *command_id,
    uint64_t timestamp_ms)
{
    UmiWorkbenchLayoutDataCommand command;
    (void)memset(&command, 0, sizeof(command));
    command.structure_size = sizeof(command);
    command.kind = kind;
    test_copy_text(command.command_id,
                   sizeof(command.command_id), command_id);
    test_copy_text(command.actor_id,
                   sizeof(command.actor_id), "developer");
    test_copy_text(command.correlation_id,
                   sizeof(command.correlation_id), "correlation");
    command.timestamp_ms = timestamp_ms;
    return command;
}

/*
 * Exercise test controller lifecycle and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_controller_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataSlaveController controller;
    UmiWorkbenchLayoutDataCommand start;
    UmiWorkbenchLayoutDataCommand health;
    UmiWorkbenchLayoutDataCommandResult result;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_init(
        &controller, service,
        "layout-persistence", "master-controller"));
    TEST_REQUIRE(controller.state ==
        UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_CREATED,
        "controller created");
    TEST_STATUS_OK(umi_workbench_layout_data_controller_initialise(
        &controller));
    start = command_base(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START,
        "command-start", 1000U);
    TEST_STATUS_OK(umi_workbench_layout_data_controller_handle(
        &controller, &start, &result));
    TEST_REQUIRE(controller.state ==
        UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING,
        "controller running");
    health = command_base(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH,
        "command-health", 1100U);
    TEST_STATUS_OK(umi_workbench_layout_data_controller_handle(
        &controller, &health, &result));
    TEST_REQUIRE(result.snapshot.started,
                 "health command returns service snapshot");
    TEST_REQUIRE(controller.handled_command_count == 2U,
                 "commands counted");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test save command and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_save_command(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataSlaveController controller;
    UmiWorkbenchLayoutDataCommand command;
    UmiWorkbenchLayoutDataCommandResult result;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.controller");
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_init(
        &controller, service,
        "layout-persistence", "master-controller"));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_initialise(
        &controller));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_start(
        &controller, 1000U));
    command = command_base(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT,
        "command-save", 2000U);
    command.payload = document;
    command.payload_size = sizeof(*document);
    command.expected_revision = 0U;
    TEST_STATUS_OK(umi_workbench_layout_data_controller_handle(
        &controller, &command, &result));
    TEST_REQUIRE(result.resulting_revision == 1U,
                 "save result revision");
    TEST_REQUIRE(result.status == UMI_STATUS_OK,
                 "save result status");
    free(document);
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test lease command and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_lease_command(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataSlaveController controller;
    UmiWorkbenchLayoutDataCommand command;
    UmiWorkbenchLayoutDataCommandResult result;
    const char client_id[] = "client-a";
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_init(
        &controller, service,
        "layout-persistence", "master-controller"));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_initialise(
        &controller));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_start(
        &controller, 1000U));
    command = command_base(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_ACQUIRE_LEASE,
        "command-lease", 2000U);
    test_copy_text(command.target_id,
                   sizeof(command.target_id), "layout.controller");
    command.payload = client_id;
    command.payload_size = sizeof(client_id);
    TEST_STATUS_OK(umi_workbench_layout_data_controller_handle(
        &controller, &command, &result));
    TEST_REQUIRE(result.fencing_token > 0U,
                 "lease token returned");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test invalid command and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_invalid_command(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataService *service = NULL;
    UmiWorkbenchLayoutDataSlaveController controller;
    UmiWorkbenchLayoutDataCommand command;
    UmiWorkbenchLayoutDataCommandResult result;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_service_create(
        server, NULL, &service));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_init(
        &controller, service,
        "layout-persistence", "master-controller"));
    TEST_STATUS_OK(umi_workbench_layout_data_controller_initialise(
        &controller));
    command = command_base(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT,
        "command-invalid", 1000U);
    TEST_REQUIRE(umi_workbench_layout_data_controller_handle(
        &controller, &command, &result) == UMI_STATUS_INVALID_STATE,
        "write before start rejected");
    TEST_REQUIRE(controller.failed_command_count == 1U,
                 "failed command counted");
    umi_workbench_layout_data_service_destroy(service);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test command text and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_command_text(void)
{
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_command_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT),
        "save-layout") == 0, "save command text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_command_kind_text(
        UMI_WORKBENCH_LAYOUT_DATA_COMMAND_ACQUIRE_LEASE),
        "acquire-lease") == 0, "lease command text");
    TEST_REQUIRE(strcmp(umi_workbench_layout_data_command_kind_text(
        (UmiWorkbenchLayoutDataCommandKind)99),
        "invalid") == 0, "invalid command text");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_controller_lifecycle() == 0,
                 "controller lifecycle");
    TEST_REQUIRE(test_save_command() == 0, "save command");
    TEST_REQUIRE(test_lease_command() == 0, "lease command");
    TEST_REQUIRE(test_invalid_command() == 0,
                 "invalid command");
    TEST_REQUIRE(test_command_text() == 0, "command text");
    return 0;
}
