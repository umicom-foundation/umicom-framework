/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/process.c
 *
 * PURPOSE:
 *   Implement the process behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process.h"

#include <stdlib.h>

/*
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_run(const char *command, int *exit_code)
{
    int code;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == 0 || command[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    code = system(command);
    /* Apply this branch only when its contract condition is satisfied. */
    if (exit_code != 0) *exit_code = code;
    return code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}
