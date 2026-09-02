/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_environment.c
 *
 * PURPOSE:
 *   Verify explicit terminal environment overrides and export.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/terminal/environment.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTerminalEnvironment *environment = NULL;
    UmiEnvironmentVariable variables[4];
    size_t count = 0U;
    assert(umi_terminal_environment_create(&environment) == UMI_STATUS_OK);
    assert(umi_terminal_environment_set(environment, "CC", "clang") == UMI_STATUS_OK);
    assert(umi_terminal_environment_set(environment, "JOBS", "8") == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_environment_get(environment, "CC"), "clang") == 0);
    assert(umi_terminal_environment_export(environment, variables, 4U, &count) == UMI_STATUS_OK);
    assert(count == 2U);
    assert(umi_terminal_environment_remove(environment, "CC") == UMI_STATUS_OK);
    assert(umi_terminal_environment_get(environment, "CC") == NULL);
    umi_terminal_environment_destroy(environment);
    return 0;
}
