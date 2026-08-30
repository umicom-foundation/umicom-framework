/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/commands.c
 *
 * PURPOSE:
 *   Implement validation and behavioural classification for typed commands handled by the Layout Slave Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/commands.h"

#include <string.h>

#include "internal.h"

void umi_workbench_layout_command_init(
    UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandKind kind,
    const char *command_id,
    const char *actor_id,
    const char *correlation_id)
{
    if (command == NULL) {
        return;
    }

    (void)memset(command, 0, sizeof(*command));
    command->structure_size = sizeof(*command);
    command->kind = kind;
    umi_workbench_layout_operation_init(
        &command->operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY,
        "embedded-operation");
    if (command_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->command_id,
            sizeof(command->command_id),
            command_id,
            true);
    }
    if (actor_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->actor_id,
            sizeof(command->actor_id),
            actor_id,
            true);
    }
    if (correlation_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->correlation_id,
            sizeof(command->correlation_id),
            correlation_id,
            true);
    }
}

bool umi_workbench_layout_command_mutates_state(
    const UmiWorkbenchLayoutCommand *command)
{
    return command != NULL &&
           command->kind >= UMI_WORKBENCH_LAYOUT_COMMAND_CREATE &&
           command->kind <= UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK;
}

bool umi_workbench_layout_command_requires_layout(
    const UmiWorkbenchLayoutCommand *command)
{
    if (command == NULL) {
        return true;
    }
    return command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_CREATE &&
           command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_CLONE &&
           command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT;
}

bool umi_workbench_layout_command_requires_unlocked_layout(
    const UmiWorkbenchLayoutCommand *command)
{
    if (command == NULL) {
        return true;
    }

    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_COMMAND_ACTIVATE:
    case UMI_WORKBENCH_LAYOUT_COMMAND_RESTORE:
    case UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT:
    case UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK:
        return false;
    default:
        return umi_workbench_layout_command_mutates_state(command);
    }
}

UmiStatus umi_workbench_layout_command_validate(
    const UmiWorkbenchLayoutCommand *command)
{
    if (command == NULL ||
        command->structure_size < sizeof(*command) ||
        command->kind < UMI_WORKBENCH_LAYOUT_COMMAND_CREATE ||
        command->kind > UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK ||
        !umi_workbench_layout_text_present(command->command_id) ||
        !umi_workbench_layout_text_present(command->actor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_workbench_layout_command_requires_layout(command) &&
        !umi_workbench_layout_text_present(command->layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_COMMAND_CREATE:
        if (!umi_workbench_layout_text_present(command->layout_id) ||
            !umi_workbench_layout_text_present(command->name)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    case UMI_WORKBENCH_LAYOUT_COMMAND_CLONE:
        if (!umi_workbench_layout_text_present(
                command->template_id) ||
            !umi_workbench_layout_text_present(command->layout_id) ||
            !umi_workbench_layout_text_present(command->name)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    case UMI_WORKBENCH_LAYOUT_COMMAND_APPLY_OPERATION:
        return umi_workbench_layout_operation_validate(
            &command->operation);

    case UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT:
        if (!umi_workbench_layout_text_present(command->text) ||
            !umi_workbench_layout_text_present(command->layout_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    case UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT:
        if (!umi_workbench_layout_text_present(command->layout_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    default:
        break;
    }
    return UMI_STATUS_OK;
}

void umi_workbench_layout_command_result_init(
    UmiWorkbenchLayoutCommandResult *result,
    const UmiWorkbenchLayoutCommand *command)
{
    if (result == NULL) {
        return;
    }

    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = UMI_STATUS_OK;
    if (command == NULL) {
        return;
    }
    result->kind = command->kind;
    (void)umi_workbench_layout_copy_text(
        result->command_id,
        sizeof(result->command_id),
        command->command_id,
        true);
    (void)umi_workbench_layout_copy_text(
        result->layout_id,
        sizeof(result->layout_id),
        command->layout_id,
        true);
}
