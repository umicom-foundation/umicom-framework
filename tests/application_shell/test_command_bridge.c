/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_command_bridge.c
 *
 * PURPOSE:
 *   Verify shell actions reconcile and execute through Framework commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application_shell/command_bridge.h"

static UmiStatus handle_command(void *user_data,
                                const char *argument,
                                char *out_message,
                                size_t message_capacity)
{
    int *calls = (int *)user_data;
    (void)argument;

    *calls += 1;
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", "executed");
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiApplicationShellRegistry *shell = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiApplicationShellContribution contribution;
    UmiCommandDescriptor descriptor;
    char message[64];
    int calls = 0;

    assert(umi_application_shell_registry_create(&shell) == UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);

    umi_application_shell_contribution_init(
        &contribution,
        "test.action",
        "Test Action",
        UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        UMI_APPLICATION_SHELL_REGION_MENU_BAR);
    (void)strcpy(contribution.command_id, "test.command");
    assert(umi_application_shell_registry_upsert(
        shell, &contribution) == UMI_STATUS_OK);

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "test.command";
    descriptor.title = "Test Command";
    descriptor.category = "Test";
    descriptor.description = "Execute test command.";
    descriptor.handler = handle_command;
    descriptor.user_data = &calls;

    assert(umi_command_registry_register(
        commands, &descriptor) == UMI_STATUS_OK);
    assert(umi_application_shell_command_bridge_reconcile(
        shell, commands) == UMI_STATUS_OK);
    assert(umi_application_shell_command_bridge_execute(
        shell,
        commands,
        "test.action",
        NULL,
        message,
        sizeof(message)) == UMI_STATUS_OK);
    assert(calls == 1);
    assert(strcmp(message, "executed") == 0);

    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(shell);
    return 0;
}
