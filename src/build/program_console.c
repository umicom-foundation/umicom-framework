/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/program_console.c
 * PURPOSE: Share project launch preparation between captured runs and interactive consoles.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/program_console.h"
#include "umicom/build/run_current.h"
#include <stdlib.h>

UmiStatus UmiBuildProgramConsoleCreate(const UmiBuildProfile *profile, bool trusted,
                                       UmiProgramConsole **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!trusted)
        return UMI_STATUS_PERMISSION_DENIED;
    if (profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* These records are deliberately heap-owned; profiles contain large path
     * fields and should not multiply stack use in GUI callbacks. */
    UmiBuildProfile *prepared = malloc(sizeof *prepared);
    UmiArguments *arguments = malloc(sizeof *arguments);
    if (prepared == NULL || arguments == NULL)
    {
        free(prepared);
        free(arguments);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = UmiBuildRunCurrentPrepare(profile, prepared);
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileArguments(prepared, arguments);
    if (status == UMI_STATUS_OK)
    {
        const char *values[UMI_ARGUMENTS_CAPACITY];
        for (size_t index = 0U; index < arguments->count; ++index)
            values[index] = arguments->values[index];
        UmiProgramConsoleConfig config = {0};
        config.command.program = prepared->run_program;
        config.command.workingDirectory = prepared->run_working_directory;
        config.command.arguments = values;
        config.command.argumentCount = arguments->count;
        config.environment = prepared->run_environment;
        config.tool_directory = prepared->tool_directory;
        config.timeout_ms = prepared->timeout_ms;
        status = UmiProgramConsoleCreate(&config, out);
    }
    free(arguments);
    free(prepared);
    return status;
}
