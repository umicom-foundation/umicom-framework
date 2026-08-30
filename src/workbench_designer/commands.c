/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/commands.c
 *
 * PURPOSE:
 *   Validate Master Controller requests before they reach the Layout Designer
 *   Slave Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/commands.h"
#include "internal.h"


void umi_workbench_designer_controller_command_init(
    UmiWorkbenchDesignerControllerCommand *command,
    UmiWorkbenchDesignerControllerCommandKind kind,
    const char *command_id)
{
    if (command == NULL) return;
    (void)memset(command, 0, sizeof(*command));
    command->kind = kind;
    command->mode = UMI_WORKBENCH_DESIGNER_MODE_DESIGN;
    command->tool = UMI_WORKBENCH_DESIGNER_TOOL_SELECT;
    if (command_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            command->command_id, sizeof(command->command_id), command_id);
    }
}

UmiStatus umi_workbench_designer_controller_command_validate(
    const UmiWorkbenchDesignerControllerCommand *command)
{
    if (command == NULL || command->command_id[0] == '\0' ||
        command->kind < UMI_WORKBENCH_DESIGNER_CONTROLLER_START ||
        command->kind > UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (command->kind >= UMI_WORKBENCH_DESIGNER_CONTROLLER_CLOSE &&
        command->kind <= UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE &&
        command->session_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_APPLY) {
        return umi_workbench_designer_command_validate(
            &command->designer_command);
    }
    return UMI_STATUS_OK;
}
