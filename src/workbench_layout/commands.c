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

/*
 * Initialise workbench layout command from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_command_init(
    UmiWorkbenchLayoutCommand *command,
    UmiWorkbenchLayoutCommandKind kind,
    const char *command_id,
    const char *actor_id,
    const char *correlation_id)
{
    /* Use the shared build helper when it is available from the parent composition. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->command_id,
            sizeof(command->command_id),
            command_id,
            true);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (actor_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->actor_id,
            sizeof(command->actor_id),
            actor_id,
            true);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (correlation_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            command->correlation_id,
            sizeof(command->correlation_id),
            correlation_id,
            true);
    }
}

/*
 * Provide the workbench layout command mutates state operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_command_mutates_state(
    const UmiWorkbenchLayoutCommand *command)
{
    return command != NULL &&
           command->kind >= UMI_WORKBENCH_LAYOUT_COMMAND_CREATE &&
           command->kind <= UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK;
}

/*
 * Provide the workbench layout command requires layout operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_command_requires_layout(
    const UmiWorkbenchLayoutCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL) {
        return true;
    }
    return command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_CREATE &&
           command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_CLONE &&
           command->kind != UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT;
}

/*
 * Provide the workbench layout command requires unlocked layout operation used by this
 * module and its client applications.
 */
bool umi_workbench_layout_command_requires_unlocked_layout(
    const UmiWorkbenchLayoutCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL) {
        return true;
    }

    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Check that workbench layout command satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_layout_command_validate(
    const UmiWorkbenchLayoutCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL ||
        command->structure_size < sizeof(*command) ||
        command->kind < UMI_WORKBENCH_LAYOUT_COMMAND_CREATE ||
        command->kind > UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK ||
        !umi_workbench_layout_text_present(command->command_id) ||
        !umi_workbench_layout_text_present(command->actor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_command_requires_layout(command) &&
        !umi_workbench_layout_text_present(command->layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (command->kind) {
    case UMI_WORKBENCH_LAYOUT_COMMAND_CREATE:
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_text_present(command->layout_id) ||
            !umi_workbench_layout_text_present(command->name)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    case UMI_WORKBENCH_LAYOUT_COMMAND_CLONE:
        /* Apply this branch only when its contract condition is satisfied. */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_text_present(command->text) ||
            !umi_workbench_layout_text_present(command->layout_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    case UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT:
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_text_present(command->layout_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;

    default:
        break;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench layout command result from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_command_result_init(
    UmiWorkbenchLayoutCommandResult *result,
    const UmiWorkbenchLayoutCommand *command)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) {
        return;
    }

    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = UMI_STATUS_OK;
    /* Use the shared build helper when it is available from the parent composition. */
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
