/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_execution.c
 *
 * PURPOSE:
 *   Implement execute-then-activate semantics for Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_execution.h"

/*
 * Provide the studio runtime execute command operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_execute_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || command_id == NULL ||
        command_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_command_registry_execute(
        bindings->commands,
        command_id,
        argument,
        out_message,
        message_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_studio_surface_activate_for_command(
        bindings,
        command_id);
}

/*
 * Provide the studio runtime execute contribution operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_execute_contribution(
    UmiStudioRuntimeBindings *bindings,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiApplicationShellContribution contribution;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || contribution_id == NULL ||
        contribution_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_registry_find(
        bindings->shell_registry,
        contribution_id,
        &contribution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_command_bridge_execute(
        bindings->shell_registry,
        bindings->commands,
        contribution_id,
        argument,
        out_message,
        message_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contribution.command_id[0] != '\0') {
        status = umi_studio_surface_activate_for_command(
            bindings,
            contribution.command_id);
    }

    return status;
}
