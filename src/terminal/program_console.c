/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/program_console.c
 * PURPOSE: Copy console launch configuration and serialize input without blocking the GUI on I/O.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "program_console_internal.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus ConsoleCopy(const char *source, size_t *budget, char **out)
{
    if (source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(source);
    if (length > *budget)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *copy = malloc(length + 1U);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, source, length + 1U);
    *budget -= length;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiProgramConsoleCreate(const UmiProgramConsoleConfig *config, UmiProgramConsole **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || config->command.argumentCount > UMI_CHANNEL_MAX_ARGUMENTS ||
        (config->command.argumentCount != 0U && config->command.arguments == NULL) ||
        !umi_path_is_absolute(config->command.program) ||
        !umi_path_is_absolute(config->command.workingDirectory))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProgramConsole *console = calloc(1U, sizeof *console);
    if (console == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t budget = 16000U;
    UmiStatus status = umi_mutex_create(&console->mutex);
    if (status == UMI_STATUS_OK)
        status = ConsoleCopy(config->command.program, &budget, &console->program);
    if (status == UMI_STATUS_OK)
        status = ConsoleCopy(config->command.workingDirectory, &budget, &console->directory);
    for (size_t index = 0U; status == UMI_STATUS_OK && index < config->command.argumentCount;
         ++index)
        status = ConsoleCopy(config->command.arguments[index], &budget, &console->arguments[index]);
    if (status == UMI_STATUS_OK)
        status = UmiProcessEnvironmentPlanCreate(config->environment, config->tool_directory,
                                                 &console->environment);
    if (status != UMI_STATUS_OK)
    {
        (void)UmiProgramConsoleDestroy(&console);
        return status;
    }
    console->command.program = console->program;
    console->command.workingDirectory = console->directory;
    console->command.arguments = (const char *const *)console->arguments;
    console->command.argumentCount = config->command.argumentCount;
    console->timeout_ms = config->timeout_ms;
    console->snapshot.exit_code = -1;
    *out = console;
    return UMI_STATUS_OK;
}
UmiStatus UmiProgramConsoleSend(UmiProgramConsole *console, const void *bytes, size_t length)
{
    if (console == NULL || bytes == NULL || length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_PROGRAM_CONSOLE_INPUT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)umi_mutex_lock(console->mutex);
    UmiStatus status = UMI_STATUS_OK;
    if (console->snapshot.completed || console->snapshot.stop_requested ||
        console->snapshot.input_ended)
        status = UMI_STATUS_INVALID_STATE;
    else if (console->count == UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY)
        status = UMI_STATUS_BUSY;
    else
    {
        size_t tail = (console->head + console->count) % UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY;
        memcpy(console->queue[tail].bytes, bytes, length);
        console->queue[tail].length = length;
        ++console->count;
        console->snapshot.queued_frames = console->count;
    }
    (void)umi_mutex_unlock(console->mutex);
    return status;
}
UmiStatus UmiProgramConsoleEndInput(UmiProgramConsole *console)
{
    if (console == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(console->mutex);
    UmiStatus status = console->snapshot.input_ended ? UMI_STATUS_OK :
        console->snapshot.completed || console->snapshot.stop_requested
            ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        console->snapshot.input_ended = true;
    (void)umi_mutex_unlock(console->mutex);
    return status;
}
UmiStatus UmiProgramConsoleStop(UmiProgramConsole *console)
{
    if (console == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(console->mutex);
    if (!console->snapshot.completed)
        console->snapshot.stop_requested = true;
    (void)umi_mutex_unlock(console->mutex);
    return UMI_STATUS_OK;
}
UmiStatus UmiProgramConsoleRead(UmiProgramConsole *console, UmiProgramConsoleSnapshot *out)
{
    if (console == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(console->mutex);
    *out = console->snapshot;
    (void)umi_mutex_unlock(console->mutex);
    return UMI_STATUS_OK;
}
UmiStatus UmiProgramConsoleDestroy(UmiProgramConsole **owner)
{
    if (owner == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProgramConsole *console = *owner;
    if (console == NULL)
        return UMI_STATUS_OK;
    if (console->mutex != NULL)
    {
        (void)umi_mutex_lock(console->mutex);
        bool active = console->entered && !console->snapshot.completed;
        (void)umi_mutex_unlock(console->mutex);
        if (active)
            return UMI_STATUS_BUSY;
    }
    UmiProcessEnvironmentPlanDestroy(console->environment);
    for (size_t index = 0U; index < UMI_CHANNEL_MAX_ARGUMENTS; ++index)
        free(console->arguments[index]);
    free(console->program);
    free(console->directory);
    umi_mutex_destroy(console->mutex);
    free(console);
    *owner = NULL;
    return UMI_STATUS_OK;
}
/* Output never shares a buffer with input; passwords sent to a child are not
 * echoed or retained as command history by this service. The child can still
 * choose to print input itself, so callers must treat output as private data. */
void ConsoleOutput(UmiProgramConsole *console, const char *bytes, size_t length)
{
    (void)umi_mutex_lock(console->mutex);
    size_t capacity = sizeof console->snapshot.output - 1U;
    size_t used = strlen(console->snapshot.output);
    uint64_t remaining = UINT64_MAX - console->snapshot.output_bytes_seen;
    console->snapshot.output_bytes_seen += length > remaining ? remaining : (uint64_t)length;
    if (length >= capacity)
    {
        bytes += length - capacity;
        length = capacity;
        used = 0U;
        console->snapshot.output_truncated = true;
    }
    if (length > capacity - used)
    {
        size_t dropped = length - (capacity - used);
        memmove(console->snapshot.output, console->snapshot.output + dropped, used - dropped);
        used -= dropped;
        console->snapshot.output_truncated = true;
    }
    for (size_t index = 0U; index < length; ++index)
        console->snapshot.output[used + index] = bytes[index] != '\0' ? bytes[index] : '?';
    console->snapshot.output[used + length] = '\0';
    (void)umi_mutex_unlock(console->mutex);
}
void ConsolePublishChannel(UmiProgramConsole *console, const UmiProcessChannelSnapshot *channel)
{
    (void)umi_mutex_lock(console->mutex);
    console->snapshot.exit_code = channel->exitCode;
    console->snapshot.diagnostics_truncated = channel->diagnosticsTruncated != 0;
    memcpy(console->snapshot.diagnostics, channel->diagnostics,
           sizeof console->snapshot.diagnostics);
    (void)umi_mutex_unlock(console->mutex);
}
