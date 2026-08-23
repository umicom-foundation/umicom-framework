/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/command_bridge.c
 *
 * PURPOSE:
 *   Keep shell actions honest: a contribution is enabled only when its command
 *   exists and the Framework command registry reports it enabled.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/command_bridge.h"

UmiStatus umi_application_shell_command_bridge_reconcile(
    UmiApplicationShellRegistry *shell,
    const UmiCommandRegistry *commands)
{
    size_t index;
    size_t count;

    if (shell == NULL || commands == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_application_shell_registry_count(shell);
    for (index = 0U; index < count; ++index) {
        UmiApplicationShellContribution contribution;
        int enabled;

        if (umi_application_shell_registry_at(
                shell, index, &contribution) != UMI_STATUS_OK) {
            return UMI_STATUS_INTERNAL_ERROR;
        }

        if (contribution.command_id[0] == '\0') {
            continue;
        }

        enabled =
            umi_command_registry_contains(commands, contribution.command_id) &&
            umi_command_registry_is_enabled(
                commands, contribution.command_id, NULL);

        if (umi_application_shell_registry_set_state(
                shell,
                contribution.contribution_id,
                (contribution.flags & UMI_APPLICATION_SHELL_VISIBLE) != 0U,
                enabled,
                (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
                contribution.badge_count) != UMI_STATUS_OK) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_command_bridge_execute(
    UmiApplicationShellRegistry *shell,
    UmiCommandRegistry *commands,
    const char *contribution_id,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiApplicationShellContribution contribution;
    UmiStatus status;

    if (shell == NULL || commands == NULL ||
        contribution_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_application_shell_registry_find(
        shell, contribution_id, &contribution);
    if (status != UMI_STATUS_OK) return status;

    if ((contribution.flags & UMI_APPLICATION_SHELL_VISIBLE) == 0U ||
        (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) == 0U) {
        return UMI_STATUS_UNAVAILABLE;
    }

    if (contribution.command_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    return umi_command_registry_execute(
        commands,
        contribution.command_id,
        argument,
        out_message,
        message_capacity);
}
