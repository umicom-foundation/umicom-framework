/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/controller.c
 *
 * PURPOSE:
 *   Implement the Layout Persistence Slave Controller lifecycle and typed command delegation under Master Controller authority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/controller.h"
#include <string.h>



const char *umi_workbench_layout_data_command_kind_text(
    UmiWorkbenchLayoutDataCommandKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START: return "start";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_QUIESCE: return "quiesce";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_STOP: return "stop";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT:
        return "save-layout";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_DELETE_LAYOUT:
        return "delete-layout";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SHARE_LAYOUT:
        return "share-layout";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_ACQUIRE_LEASE:
        return "acquire-lease";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_CHECKPOINT_SESSION:
        return "checkpoint-session";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_BACKUP: return "backup";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_RESTORE: return "restore";
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH: return "health";
    default: return "invalid";
    }
}

UmiStatus umi_workbench_layout_data_controller_init(
    UmiWorkbenchLayoutDataSlaveController *controller,
    UmiWorkbenchLayoutDataService *service,
    const char *controller_id,
    const char *master_controller_id)
{
    UmiStatus status;
    if (controller == NULL || service == NULL ||
        controller_id == NULL || master_controller_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(controller, 0, sizeof(*controller));
    controller->structure_size = sizeof(*controller);
    controller->service = service;
    controller->state =
        UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_CREATED;
    controller->revision = 1U;
    status = umi_workbench_layout_data_copy_text(
        controller->controller_id,
        sizeof(controller->controller_id),
        controller_id, false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            controller->master_controller_id,
            sizeof(controller->master_controller_id),
            master_controller_id, false);
    }
    return status;
}

UmiStatus umi_workbench_layout_data_controller_initialise(
    UmiWorkbenchLayoutDataSlaveController *controller)
{
    if (controller == NULL ||
        controller->state !=
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_CREATED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state =
        UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_INITIALISED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_controller_start(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms)
{
    UmiStatus status;
    if (controller == NULL ||
        (controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_INITIALISED &&
         controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_start(
        controller->service, now_ms);
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING;
        controller->revision += 1U;
    } else {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED;
    }
    return status;
}

UmiStatus umi_workbench_layout_data_controller_quiesce(
    UmiWorkbenchLayoutDataSlaveController *controller)
{
    UmiStatus status;
    if (controller == NULL ||
        controller->state !=
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_quiesce(
        controller->service);
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED;
        controller->revision += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_data_controller_stop(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms)
{
    UmiStatus status;
    if (controller == NULL ||
        (controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING &&
         controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_stop(
        controller->service, now_ms);
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED;
        controller->revision += 1U;
    } else {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED;
    }
    return status;
}

static UmiStatus validate_command(
    const UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command)
{
    if (controller == NULL || command == NULL ||
        command->structure_size < sizeof(*command) ||
        command->command_id[0] == '\0' ||
        command->actor_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (command->kind != UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START &&
        command->kind != UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH &&
        controller->state !=
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus handle_running_command(
    UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command,
    UmiWorkbenchLayoutDataCommandResult *result)
{
    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_QUIESCE:
        return umi_workbench_layout_data_controller_quiesce(controller);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_STOP:
        return umi_workbench_layout_data_controller_stop(
            controller, command->timestamp_ms);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT:
        if (command->payload == NULL ||
            command->payload_size <
                sizeof(UmiWorkbenchLayoutDocument)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        return umi_workbench_layout_data_service_save_layout(
            controller->service,
            (UmiWorkbenchLayoutDocument *)command->payload,
            command->expected_revision,
            command->correlation_id,
            command->timestamp_ms,
            &result->resulting_revision);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_DELETE_LAYOUT:
        return umi_workbench_layout_data_service_delete_layout(
            controller->service,
            command->target_id,
            command->expected_revision,
            command->correlation_id,
            command->timestamp_ms);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SHARE_LAYOUT:
        if (command->payload == NULL ||
            command->payload_size <
                sizeof(UmiWorkbenchLayoutGrant)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        return umi_workbench_layout_data_service_share_layout(
            controller->service,
            (const UmiWorkbenchLayoutGrant *)command->payload,
            command->correlation_id,
            command->timestamp_ms);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_ACQUIRE_LEASE:
        {
            UmiWorkbenchLayoutLease lease;
            const char *client_id =
                command->payload != NULL
                    ? (const char *)command->payload : "";
            UmiStatus status =
                umi_workbench_layout_data_service_acquire_lease(
                    controller->service,
                    command->target_id,
                    command->actor_id,
                    client_id,
                    command->timestamp_ms,
                    &lease);
            if (status == UMI_STATUS_OK) {
                result->fencing_token = lease.fencing_token;
            }
            return status;
        }
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_CHECKPOINT_SESSION:
        if (command->payload == NULL ||
            command->payload_size <
                sizeof(UmiWorkbenchLayoutSession)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        return umi_workbench_layout_data_service_checkpoint_session(
            controller->service,
            (const UmiWorkbenchLayoutSession *)command->payload,
            command->expected_revision,
            &result->resulting_revision);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH:
        return umi_workbench_layout_data_service_snapshot(
            controller->service,
            command->timestamp_ms,
            &result->snapshot);
    default:
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
}

UmiStatus umi_workbench_layout_data_controller_handle(
    UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command,
    UmiWorkbenchLayoutDataCommandResult *out_result)
{
    UmiWorkbenchLayoutDataCommandResult result;
    UmiStatus status;
    (void)memset(&result, 0, sizeof(result));
    result.structure_size = sizeof(result);
    status = validate_command(controller, command);
    if (status == UMI_STATUS_OK &&
        command->kind == UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START) {
        status = umi_workbench_layout_data_controller_start(
            controller, command->timestamp_ms);
    } else if (status == UMI_STATUS_OK) {
        status = handle_running_command(
            controller, command, &result);
    }
    result.status = status;
    (void)umi_workbench_layout_data_copy_text(
        result.message, sizeof(result.message),
        status == UMI_STATUS_OK
            ? "Layout persistence command completed."
            : "Layout persistence command failed.",
        true);
    if (controller != NULL) {
        controller->handled_command_count += 1U;
        if (status != UMI_STATUS_OK) {
            controller->failed_command_count += 1U;
        }
        controller->revision += 1U;
    }
    if (out_result != NULL) *out_result = result;
    return status;
}
