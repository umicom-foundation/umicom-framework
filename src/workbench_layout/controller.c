/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/controller.c
 *
 * PURPOSE:
 *   Implement the Layout Slave Controller lifecycle and route typed layout
 *   commands to the authoritative Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/controller.h"

#include <stdlib.h>
#include <string.h>

#include "internal.h"

struct UmiWorkbenchLayoutSlaveController {
    UmiWorkbenchLayoutControllerConfig config;
    UmiWorkbenchLayoutService *service;
    UmiWorkbenchLayoutControllerState state;
    uint64_t accepted_command_count;
    uint64_t rejected_command_count;
    uint64_t failed_command_count;
    uint64_t revision;
};

/*
 * Provide the result set message operation used by this module and its client
 * applications.
 */
static void result_set_message(
    UmiWorkbenchLayoutCommandResult *result,
    const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) {
        return;
    }
    (void)umi_workbench_layout_copy_text(
        result->message,
        sizeof(result->message),
        message != NULL ? message : "",
        true);
}

/* Provide the controller reject operation used by this module and its client applications. */
static UmiStatus controller_reject(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutCommand *command,
    UmiStatus status,
    const char *message,
    UmiWorkbenchLayoutCommandResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller != NULL) {
        controller->rejected_command_count += 1U;
        controller->revision += 1U;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result != NULL) {
        umi_workbench_layout_command_result_init(result, command);
        result->status = status;
        result_set_message(result, message);
    }
    return status;
}

/* Provide the make identity operation used by this module and its client applications. */
static UmiStatus make_identity(
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutIdentity *identity)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (principal == NULL || command == NULL || identity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(identity, 0, sizeof(*identity));
    status = umi_workbench_layout_copy_text(
        identity->layout_id,
        sizeof(identity->layout_id),
        command->layout_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            identity->owner_user_id,
            sizeof(identity->owner_user_id),
            principal->user_id,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            identity->owner_application_id,
            sizeof(identity->owner_application_id),
            "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            identity->workspace_id,
            sizeof(identity->workspace_id),
            principal->workspace_id,
            true);
    }
    return status;
}

/* Initialise handle from caller-provided values so later operations receive a known state. */
static UmiStatus handle_create(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutDocument *document;
    UmiStatus status;

    status = make_identity(principal, command, &identity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        document = (UmiWorkbenchLayoutDocument *)
            calloc(1U, sizeof(*document));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (document == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = umi_workbench_layout_service_create_layout(
            controller->service,
            principal,
            &identity,
            command->name,
            document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            result->changed = true;
            result->previous_revision = 0U;
            result->resulting_revision = document->version.revision;
        }
        free(document);
    }
    return status;
}

/* Provide the handle clone operation used by this module and its client applications. */
static UmiStatus handle_clone(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiWorkbenchLayoutIdentity identity;
    UmiWorkbenchLayoutDocument *document;
    UmiStatus status;

    status = make_identity(principal, command, &identity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        document = (UmiWorkbenchLayoutDocument *)
            calloc(1U, sizeof(*document));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (document == NULL) {
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = umi_workbench_layout_service_clone_template(
            controller->service,
            principal,
            command->template_id,
            &identity,
            command->name,
            document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            result->changed = true;
            result->previous_revision = 0U;
            result->resulting_revision = document->version.revision;
        }
        free(document);
    }
    return status;
}

/*
 * Provide the handle apply operation operation used by this module and its client
 * applications.
 */
static UmiStatus handle_apply_operation(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiWorkbenchLayoutOperation operation;
    UmiWorkbenchLayoutOperationResult operation_result;
    UmiStatus status;

    operation = command->operation;
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation.timestamp_ms == 0U) {
        operation.timestamp_ms = command->timestamp_ms;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation.expected_revision == 0U) {
        operation.expected_revision = command->expected_revision;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_layout_text_present(operation.actor_id)) {
        (void)umi_workbench_layout_copy_text(
            operation.actor_id,
            sizeof(operation.actor_id),
            command->actor_id,
            true);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_layout_text_present(operation.correlation_id)) {
        (void)umi_workbench_layout_copy_text(
            operation.correlation_id,
            sizeof(operation.correlation_id),
            command->correlation_id,
            true);
    }

    status = umi_workbench_layout_service_apply_operation(
        controller->service,
        principal,
        &operation,
        &operation_result);
    result->changed = operation_result.changed;
    result->previous_revision = operation_result.previous_revision;
    result->resulting_revision = operation_result.resulting_revision;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (operation_result.message[0] != '\0') {
        result_set_message(result, operation_result.message);
    }
    return status;
}

/* Provide the handle lock state operation used by this module and its client applications. */
static UmiStatus handle_lock_state(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    bool locked,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiWorkbenchLayoutOperation operation;
    UmiWorkbenchLayoutOperationResult operation_result;
    UmiStatus status;

    umi_workbench_layout_operation_init(
        &operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED,
        command->command_id);
    (void)umi_workbench_layout_copy_text(
        operation.actor_id,
        sizeof(operation.actor_id),
        command->actor_id,
        true);
    (void)umi_workbench_layout_copy_text(
        operation.correlation_id,
        sizeof(operation.correlation_id),
        command->correlation_id,
        true);
    operation.expected_revision = command->expected_revision;
    operation.timestamp_ms = command->timestamp_ms;
    operation.bool_value = locked;

    status = umi_workbench_layout_service_apply_operation(
        controller->service,
        principal,
        &operation,
        &operation_result);
    result->changed = operation_result.changed;
    result->previous_revision = operation_result.previous_revision;
    result->resulting_revision = operation_result.resulting_revision;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (operation_result.message[0] != '\0') {
        result_set_message(result, operation_result.message);
    }
    return status;
}

/*
 * Write handle in its stable representation and report capacity or input failures to the
 * caller.
 */
static UmiStatus handle_save(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiWorkbenchLayoutPersistenceResult persistence_result;
    UmiStatus status;

    status = umi_workbench_layout_service_save(
        controller->service,
        principal,
        command->actor_id,
        command->correlation_id,
        command->timestamp_ms,
        &persistence_result);
    result->previous_revision = persistence_result.previous_revision;
    result->resulting_revision = persistence_result.resulting_revision;
    result->changed = status == UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (persistence_result.message[0] != '\0') {
        result_set_message(result, persistence_result.message);
    }
    return status;
}

/* Provide the handle restore operation used by this module and its client applications. */
static UmiStatus handle_restore(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    UmiStatus status;

    status = umi_workbench_layout_service_load(
        controller->service,
        principal,
        command->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_service_activate(
            controller->service,
            principal,
            command->layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const UmiWorkbenchLayoutDocument *document =
            umi_workbench_layout_service_active_layout(
                controller->service);
        result->changed = true;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (document != NULL) {
            result->previous_revision = document->version.base_revision;
            result->resulting_revision = document->version.revision;
        }
    }
    return status;
}

/* Provide the dispatch command operation used by this module and its client applications. */
static UmiStatus dispatch_command(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *result)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_COMMAND_CREATE:
        return handle_create(controller, principal, command, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_CLONE:
        return handle_clone(controller, principal, command, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_DELETE:
        result->previous_revision = command->expected_revision;
        result->changed = true;
        return umi_workbench_layout_service_delete(
            controller->service,
            principal,
            command->layout_id,
            command->expected_revision);
    case UMI_WORKBENCH_LAYOUT_COMMAND_ACTIVATE:
        result->changed = true;
        return umi_workbench_layout_service_activate(
            controller->service,
            principal,
            command->layout_id);
    case UMI_WORKBENCH_LAYOUT_COMMAND_APPLY_OPERATION:
        return handle_apply_operation(
            controller, principal, command, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_UNDO:
        result->changed = true;
        return umi_workbench_layout_service_undo(
            controller->service, principal);
    case UMI_WORKBENCH_LAYOUT_COMMAND_REDO:
        result->changed = true;
        return umi_workbench_layout_service_redo(
            controller->service, principal);
    case UMI_WORKBENCH_LAYOUT_COMMAND_SAVE:
        return handle_save(controller, principal, command, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_RESTORE:
        return handle_restore(controller, principal, command, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_LOCK:
        return handle_lock_state(
            controller, principal, command, true, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK:
        return handle_lock_state(
            controller, principal, command, false, result);
    case UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT:
    case UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT:
        result_set_message(
            result,
            "Import and export require an explicit stream or file adapter; "
            "the controller does not place complete layout documents inside "
            "a bounded command string.");
        return UMI_STATUS_NOT_IMPLEMENTED;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
}

/*
 * Provide the workbench layout controller config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutControllerConfig
umi_workbench_layout_controller_config_default(void)
{
    UmiWorkbenchLayoutControllerConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    (void)umi_workbench_layout_copy_text(
        config.controller_id,
        sizeof(config.controller_id),
        "umicom.workbench-layout.slave-controller",
        false);
    config.reject_commands_while_quiesced = true;
    config.stop_on_persistence_failure = false;
    config.emit_command_events = true;
    return config;
}

/*
 * Initialise workbench layout slave controller from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_slave_controller_create(
    const UmiWorkbenchLayoutControllerConfig *config,
    UmiWorkbenchLayoutService *service,
    UmiWorkbenchLayoutSlaveController **out_controller)
{
    UmiWorkbenchLayoutControllerConfig effective;
    UmiWorkbenchLayoutSlaveController *controller;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_controller = NULL;
    effective = config != NULL
        ? *config
        : umi_workbench_layout_controller_config_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective) ||
        !umi_workbench_layout_text_present(effective.controller_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    controller = (UmiWorkbenchLayoutSlaveController *)
        calloc(1U, sizeof(*controller));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    controller->config = effective;
    controller->service = service;
    controller->state = UMI_WORKBENCH_LAYOUT_CONTROLLER_CREATED;
    controller->revision = 1U;
    *out_controller = controller;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by workbench layout slave controller so the same storage can
 * be reused safely.
 */
void umi_workbench_layout_slave_controller_destroy(
    UmiWorkbenchLayoutSlaveController *controller)
{
    free(controller);
}

/*
 * Provide the workbench layout slave controller initialise operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_initialise(
    UmiWorkbenchLayoutSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state ==
        UMI_WORKBENCH_LAYOUT_CONTROLLER_INITIALISED) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
        UMI_WORKBENCH_LAYOUT_CONTROLLER_CREATED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state =
        UMI_WORKBENCH_LAYOUT_CONTROLLER_INITIALISED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout slave controller start operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_start(
    UmiWorkbenchLayoutSlaveController *controller)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state ==
        UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
        UMI_WORKBENCH_LAYOUT_CONTROLLER_INITIALISED &&
        controller->state !=
        UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_workbench_layout_service_start(
        controller->service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED;
        controller->revision += 1U;
        return status;
    }
    controller->state =
        UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout slave controller quiesce operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_quiesce(
    UmiWorkbenchLayoutSlaveController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state ==
        UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
        UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state =
        UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout slave controller stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_stop(
    UmiWorkbenchLayoutSlaveController *controller,
    uint64_t timestamp_ms)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state ==
        UMI_WORKBENCH_LAYOUT_CONTROLLER_STOPPED) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
            UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING &&
        controller->state !=
            UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED &&
        controller->state !=
            UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_service_stop(
        controller->service, timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK &&
        controller->config.stop_on_persistence_failure) {
        controller->state =
            UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED;
        controller->revision += 1U;
        return status;
    }
    controller->state =
        UMI_WORKBENCH_LAYOUT_CONTROLLER_STOPPED;
    controller->revision += 1U;
    return status;
}

/*
 * Perform workbench layout slave controller through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_slave_controller_handle(
    UmiWorkbenchLayoutSlaveController *controller,
    const UmiWorkbenchLayoutPrincipal *principal,
    const UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandResult *out_result)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || principal == NULL ||
        command == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_command_result_init(
        out_result, command);

    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state ==
            UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED &&
        controller->config.reject_commands_while_quiesced) {
        return controller_reject(
            controller,
            command,
            UMI_STATUS_BUSY,
            "The Layout Slave Controller is quiesced and is not accepting "
            "new commands.",
            out_result);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state !=
        UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING) {
        return controller_reject(
            controller,
            command,
            UMI_STATUS_INVALID_STATE,
            "The Layout Slave Controller is not running.",
            out_result);
    }

    status = umi_workbench_layout_command_validate(command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return controller_reject(
            controller,
            command,
            status,
            "The layout command failed structural validation.",
            out_result);
    }

    controller->accepted_command_count += 1U;
    status = dispatch_command(
        controller, principal, command, out_result);
    out_result->status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        controller->failed_command_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (out_result->message[0] == '\0') {
            result_set_message(
                out_result,
                umi_status_text(status));
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_IO_ERROR &&
            controller->config.stop_on_persistence_failure) {
            controller->state =
                UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED;
        }
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (out_result->message[0] == '\0') {
        result_set_message(
            out_result,
            "The layout command completed successfully.");
    }
    controller->revision += 1U;
    return status;
}

/*
 * Provide the workbench layout slave controller snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_slave_controller_snapshot(
    const UmiWorkbenchLayoutSlaveController *controller,
    UmiWorkbenchLayoutControllerSnapshot *out_snapshot)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    (void)umi_workbench_layout_copy_text(
        out_snapshot->controller_id,
        sizeof(out_snapshot->controller_id),
        controller->config.controller_id,
        true);
    out_snapshot->state = controller->state;
    out_snapshot->accepted_command_count =
        controller->accepted_command_count;
    out_snapshot->rejected_command_count =
        controller->rejected_command_count;
    out_snapshot->failed_command_count =
        controller->failed_command_count;
    out_snapshot->revision = controller->revision;
    status = umi_workbench_layout_service_snapshot(
        controller->service, &out_snapshot->service);
    return status;
}

/*
 * Provide the workbench layout slave controller state operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutControllerState
umi_workbench_layout_slave_controller_state(
    const UmiWorkbenchLayoutSlaveController *controller)
{
    return controller != NULL
        ? controller->state
        : UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED;
}
