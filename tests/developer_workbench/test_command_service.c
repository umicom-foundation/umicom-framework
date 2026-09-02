/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_command_service.c
 *
 * PURPOSE:
 *   Verify external commands remain disabled until explicitly bound, preventing
 *   non-functional menus from pretending an operation is available.
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

#include "umicom/developer_workbench/action_registry.h"

/*
 * Exercise execute action and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus execute_action(void *user_data,
                                const char *argument,
                                char *out_message,
                                size_t message_capacity)
{
    int *calls = (int *)user_data;
    (void)argument;

    *calls += 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", "bound");
    }
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperWorkbenchActionRegistry *actions = NULL;
    char message[32];
    int calls = 0;

    assert(umi_developer_workbench_action_registry_create(&actions) ==
           UMI_STATUS_OK);

    assert(!umi_developer_workbench_action_registry_is_enabled(
        actions, "file.save", NULL));

    assert(umi_developer_workbench_action_registry_bind(
        actions,
        "file.save",
        execute_action,
        NULL,
        &calls) == UMI_STATUS_OK);

    assert(umi_developer_workbench_action_registry_is_enabled(
        actions, "file.save", NULL));

    assert(umi_developer_workbench_action_registry_execute(
        actions,
        "file.save",
        NULL,
        message,
        sizeof(message)) == UMI_STATUS_OK);
    assert(calls == 1);
    assert(strcmp(message, "bound") == 0);

    umi_developer_workbench_action_registry_destroy(actions);
    return 0;
}
