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

UmiStatus umi_process_run(const char *command, int *exit_code)
{
    int code;
    if (command == 0 || command[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    code = system(command);
    if (exit_code != 0) *exit_code = code;
    return code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}
