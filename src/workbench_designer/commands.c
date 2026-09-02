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


/*
 * Initialise workbench designer controller command from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_controller_command_init(
    UmiWorkbenchDesignerControllerCommand *command,
    UmiWorkbenchDesignerControllerCommandKind kind,
    const char *command_id)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL) return;
    (void)memset(command, 0, sizeof(*command));
    command->kind = kind;
    command->mode = UMI_WORKBENCH_DESIGNER_MODE_DESIGN;
    command->tool = UMI_WORKBENCH_DESIGNER_TOOL_SELECT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            command->command_id, sizeof(command->command_id), command_id);
    }
}

/*
 * Check that workbench designer controller command satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_designer_controller_command_validate(
    const UmiWorkbenchDesignerControllerCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL || command->command_id[0] == '\0' ||
        command->kind < UMI_WORKBENCH_DESIGNER_CONTROLLER_START ||
        command->kind > UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind >= UMI_WORKBENCH_DESIGNER_CONTROLLER_CLOSE &&
        command->kind <= UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE &&
        command->session_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind == UMI_WORKBENCH_DESIGNER_CONTROLLER_APPLY) {
        return umi_workbench_designer_command_validate(
            &command->designer_command);
    }
    return UMI_STATUS_OK;
}
