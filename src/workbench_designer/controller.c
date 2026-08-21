/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/controller.c
 *
 * PURPOSE:
 *   Route authorised commands to the Framework-owned designer service while
 *   preserving lifecycle and failure evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/controller.h"
#include "internal.h"


void umi_workbench_designer_controller_init(
    UmiWorkbenchDesignerController *controller,
    const char *controller_id,
    UmiWorkbenchDesignerService *service)
{
    if (controller == NULL) return;
    (void)memset(controller, 0, sizeof(*controller));
    controller->state = UMI_WORKBENCH_DESIGNER_STATE_CREATED;
    controller->service = service;
    controller->last_status = UMI_STATUS_OK;
    if (controller_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            controller->controller_id,
            sizeof(controller->controller_id), controller_id);
    }
}

UmiStatus umi_workbench_designer_controller_initialise(
    UmiWorkbenchDesignerController *controller)
{
    if (controller == NULL || controller->service == NULL ||
        controller->controller_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_WORKBENCH_DESIGNER_STATE_CREATED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_WORKBENCH_DESIGNER_STATE_INITIALISED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_controller_start(
    UmiWorkbenchDesignerController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_WORKBENCH_DESIGNER_STATE_INITIALISED &&
        controller->state != UMI_WORKBENCH_DESIGNER_STATE_QUIESCED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_WORKBENCH_DESIGNER_STATE_RUNNING;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_controller_quiesce(
    UmiWorkbenchDesignerController *controller)
{
    if (controller == NULL ||
        controller->state != UMI_WORKBENCH_DESIGNER_STATE_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_WORKBENCH_DESIGNER_STATE_QUIESCED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_controller_stop(
    UmiWorkbenchDesignerController *controller)
{
    if (controller == NULL ||
        (controller->state != UMI_WORKBENCH_DESIGNER_STATE_RUNNING &&
         controller->state != UMI_WORKBENCH_DESIGNER_STATE_QUIESCED &&
         controller->state != UMI_WORKBENCH_DESIGNER_STATE_INITIALISED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_WORKBENCH_DESIGNER_STATE_STOPPED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

static void controller_result_init(
    UmiWorkbenchDesignerControllerResult *result,
    const UmiWorkbenchDesignerControllerCommand *command,
    const UmiWorkbenchDesignerService *service)
{
    (void)memset(result, 0, sizeof(*result));
    result->status = UMI_STATUS_OK;
    result->service_revision =
        umi_workbench_designer_service_revision(service);
    if (command->session_id[0] != '\0') {
        (void)umi_workbench_designer_copy_text(
            result->session_id, sizeof(result->session_id),
            command->session_id);
    }
}

UmiStatus umi_workbench_designer_controller_dispatch(
    UmiWorkbenchDesignerController *controller,
    const UmiWorkbenchDesignerControllerCommand *command,
    UmiWorkbenchDesignerControllerResult *out_result)
{
    UmiWorkbenchDesignerSession *session;
    UmiWorkbenchDesignerCommandResult command_result;
    UmiStatus status;
    if (controller == NULL || command == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_designer_controller_command_validate(command);
    if (status != UMI_STATUS_OK) return status;
    controller_result_init(out_result, command, controller->service);
    if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_START) {
        status = umi_workbench_designer_controller_start(controller);
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_QUIESCE) {
        status = umi_workbench_designer_controller_quiesce(controller);
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_STOP) {
        status = umi_workbench_designer_controller_stop(controller);
    } else {
        if (controller->state != UMI_WORKBENCH_DESIGNER_STATE_RUNNING) {
            status = UMI_STATUS_INVALID_STATE;
        } else {
            session = umi_workbench_designer_service_find(
                controller->service, command->session_id);
            if (session == NULL) status = UMI_STATUS_NOT_FOUND;
            else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_CLOSE) {
                status = umi_workbench_designer_service_close(
                    controller->service,
                    command->session_id,
                    command->discard_dirty);
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_ACTIVATE) {
                status = umi_workbench_designer_service_activate(
                    controller->service, command->session_id);
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_APPLY) {
                status = umi_workbench_designer_session_apply(
                    session, &command->designer_command, &command_result);
                out_result->changed = command_result.document_changed;
                (void)umi_workbench_designer_copy_text(
                    out_result->message, sizeof(out_result->message),
                    command_result.message);
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_SET_MODE) {
                status = umi_workbench_designer_session_set_mode(
                    session, command->mode);
                out_result->changed = status == UMI_STATUS_OK;
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_SET_TOOL) {
                status = umi_workbench_designer_session_set_tool(
                    session, command->tool);
                out_result->changed = status == UMI_STATUS_OK;
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_SELECT) {
                status = umi_workbench_designer_session_select(
                    session,
                    command->designer_command.target_node_id,
                    command->extend_selection,
                    command->toggle_selection);
                out_result->changed = status == UMI_STATUS_OK;
            } else if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE) {
                UmiWorkbenchDesignerAutosave *autosave =
                    umi_workbench_designer_session_autosave(session);
                status = umi_workbench_designer_autosave_begin(
                    autosave, command->timestamp_ms);
                out_result->changed = status == UMI_STATUS_OK;
            } else {
                status = UMI_STATUS_NOT_IMPLEMENTED;
            }
        }
    }
    out_result->status = status;
    out_result->service_revision =
        umi_workbench_designer_service_revision(controller->service);
    if (out_result->message[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message),
            status == UMI_STATUS_OK
                ? "Layout Designer Slave Controller command completed."
                : "Layout Designer Slave Controller command failed.");
    }
    controller->handled_command_count += 1U;
    controller->last_status = status;
    if (status != UMI_STATUS_OK) {
        controller->failed_command_count += 1U;
        (void)umi_workbench_designer_copy_text(
            controller->last_error, sizeof(controller->last_error),
            out_result->message);
    } else {
        controller->last_error[0] = '\0';
    }
    controller->revision += 1U;
    return status;
}
