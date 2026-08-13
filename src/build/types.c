/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/types.c
 *
 * PURPOSE:
 *   Implement stable build enum text and owned command construction helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/types.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U || source == NULL) {
        return 0;
    }
    if (strlen(source) + 1U > capacity) {
        return 0;
    }
    (void)memcpy(destination, source, strlen(source) + 1U);
    return 1;
}

const char *umi_build_phase_text(UmiBuildPhase phase)
{
    switch (phase) {
        case UMI_BUILD_PHASE_CONFIGURE: return "configure";
        case UMI_BUILD_PHASE_BUILD: return "build";
        case UMI_BUILD_PHASE_TEST: return "test";
        case UMI_BUILD_PHASE_CLEAN: return "clean";
        case UMI_BUILD_PHASE_RUN: return "run";
        case UMI_BUILD_PHASE_INSTALL: return "install";
        default: return "unknown";
    }
}

const char *umi_build_state_text(UmiBuildState state)
{
    switch (state) {
        case UMI_BUILD_STATE_CREATED: return "created";
        case UMI_BUILD_STATE_RUNNING: return "running";
        case UMI_BUILD_STATE_SUCCEEDED: return "succeeded";
        case UMI_BUILD_STATE_FAILED: return "failed";
        case UMI_BUILD_STATE_CANCELLED: return "cancelled";
        case UMI_BUILD_STATE_TIMED_OUT: return "timed-out";
        default: return "unknown";
    }
}

const char *umi_build_node_state_text(UmiBuildNodeState state)
{
    switch (state) {
        case UMI_BUILD_NODE_PENDING: return "pending";
        case UMI_BUILD_NODE_READY: return "ready";
        case UMI_BUILD_NODE_RUNNING: return "running";
        case UMI_BUILD_NODE_SUCCEEDED: return "succeeded";
        case UMI_BUILD_NODE_FAILED: return "failed";
        case UMI_BUILD_NODE_CANCELLED: return "cancelled";
        case UMI_BUILD_NODE_TIMED_OUT: return "timed-out";
        case UMI_BUILD_NODE_SKIPPED: return "skipped";
        case UMI_BUILD_NODE_BLOCKED: return "blocked";
        default: return "unknown";
    }
}

const char *umi_build_diagnostic_severity_text(
    UmiBuildDiagnosticSeverity severity)
{
    switch (severity) {
        case UMI_BUILD_DIAGNOSTIC_NOTE: return "note";
        case UMI_BUILD_DIAGNOSTIC_WARNING: return "warning";
        case UMI_BUILD_DIAGNOSTIC_ERROR: return "error";
        case UMI_BUILD_DIAGNOSTIC_FATAL: return "fatal";
        default: return "unknown";
    }
}

void umi_build_command_init(UmiBuildCommand *command, const char *program)
{
    if (command == NULL) {
        return;
    }
    (void)memset(command, 0, sizeof(*command));
    if (program != NULL) {
        (void)copy_text(command->program, sizeof(command->program), program);
    }
}

int umi_build_command_add_argument(UmiBuildCommand *command,
                                   const char *argument)
{
    size_t index;
    if (command == NULL || argument == NULL ||
        command->argument_count >= UMI_BUILD_MAX_ARGUMENTS) {
        return 0;
    }
    index = command->argument_count;
    if (!copy_text(command->argument_storage[index],
                   sizeof(command->argument_storage[index]),
                   argument)) {
        return 0;
    }
    command->arguments[index] = command->argument_storage[index];
    command->argument_count += 1U;
    return 1;
}

int umi_build_command_set_working_directory(UmiBuildCommand *command,
                                            const char *directory)
{
    if (command == NULL || directory == NULL) {
        return 0;
    }
    return copy_text(command->working_directory,
                     sizeof(command->working_directory),
                     directory);
}

int umi_build_command_format(const UmiBuildCommand *command,
                             char *out_text,
                             size_t capacity)
{
    size_t index;
    size_t used;
    int written;
    if (command == NULL || out_text == NULL || capacity == 0U ||
        command->program[0] == '\0') {
        return 0;
    }
    written = snprintf(out_text, capacity, "%s", command->program);
    if (written < 0 || (size_t)written >= capacity) {
        return 0;
    }
    used = (size_t)written;
    for (index = 0U; index < command->argument_count; ++index) {
        written = snprintf(out_text + used,
                           capacity - used,
                           " \"%s\"",
                           command->arguments[index]);
        if (written < 0 || (size_t)written >= capacity - used) {
            return 0;
        }
        used += (size_t)written;
    }
    return 1;
}
