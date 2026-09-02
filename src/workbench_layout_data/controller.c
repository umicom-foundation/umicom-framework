/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/controller.c
 *
 * PURPOSE:
 *   Implement the Layout Persistence Slave Controller lifecycle and typed command delegation under Master Controller authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/controller.h"
#include <string.h>



/*
 * Provide the workbench layout data command kind text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_layout_data_command_kind_text(
    UmiWorkbenchLayoutDataCommandKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Initialise workbench layout data controller from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_data_controller_init(
    UmiWorkbenchLayoutDataSlaveController *controller,
    UmiWorkbenchLayoutDataService *service,
    const char *controller_id,
    const char *master_controller_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            controller->master_controller_id,
            sizeof(controller->master_controller_id),
            master_controller_id, false);
    }
    return status;
}

/*
 * Provide the workbench layout data controller initialise operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_controller_initialise(
    UmiWorkbenchLayoutDataSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workbench layout data controller start operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_controller_start(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL ||
        (controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_INITIALISED &&
         controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_start(
        controller->service, now_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING;
        controller->revision += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED;
    }
    return status;
}

/*
 * Provide the workbench layout data controller quiesce operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_controller_quiesce(
    UmiWorkbenchLayoutDataSlaveController *controller)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL ||
        controller->state !=
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_quiesce(
        controller->service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED;
        controller->revision += 1U;
    }
    return status;
}

/*
 * Provide the workbench layout data controller stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_controller_stop(
    UmiWorkbenchLayoutDataSlaveController *controller,
    uint64_t now_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL ||
        (controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING &&
         controller->state !=
             UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_QUIESCED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_data_service_stop(
        controller->service, now_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_STOPPED;
        controller->revision += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        controller->state =
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_FAILED;
    }
    return status;
}

/* Provide the validate command operation used by this module and its client applications. */
static UmiStatus validate_command(
    const UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || command == NULL ||
        command->structure_size < sizeof(*command) ||
        command->command_id[0] == '\0' ||
        command->actor_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind != UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START &&
        command->kind != UMI_WORKBENCH_LAYOUT_DATA_COMMAND_HEALTH &&
        controller->state !=
            UMI_WORKBENCH_LAYOUT_DATA_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the handle running command operation used by this module and its client
 * applications.
 */
static UmiStatus handle_running_command(
    UmiWorkbenchLayoutDataSlaveController *controller,
    const UmiWorkbenchLayoutDataCommand *command,
    UmiWorkbenchLayoutDataCommandResult *result)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_QUIESCE:
        return umi_workbench_layout_data_controller_quiesce(controller);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_STOP:
        return umi_workbench_layout_data_controller_stop(
            controller, command->timestamp_ms);
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_SAVE_LAYOUT:
        /* Use the shared build helper when it is available from the parent composition. */
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
        /* Use the shared build helper when it is available from the parent composition. */
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
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                result->fencing_token = lease.fencing_token;
            }
            return status;
        }
    case UMI_WORKBENCH_LAYOUT_DATA_COMMAND_CHECKPOINT_SESSION:
        /* Use the shared build helper when it is available from the parent composition. */
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

/*
 * Perform workbench layout data controller through the module contract so client
 * applications do not duplicate its policy.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        command->kind == UMI_WORKBENCH_LAYOUT_DATA_COMMAND_START) {
        status = umi_workbench_layout_data_controller_start(
            controller, command->timestamp_ms);
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller != NULL) {
        controller->handled_command_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            controller->failed_command_count += 1U;
        }
        controller->revision += 1U;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) *out_result = result;
    return status;
}
