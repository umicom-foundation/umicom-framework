/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/types.c
 *
 * PURPOSE:
 *   Implement stable build enum text, owned argv command construction, portable
 *   shell-independent parsing and safe diagnostic formatting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/types.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return 0;
    }

    length = strlen(source);
    if (length + 1U > capacity) {
        return 0;
    }

    (void)memcpy(destination, source, length + 1U);
    return 1;
}

static void write_message(char *out_message,
                          size_t capacity,
                          const char *message)
{
    if (out_message == NULL || capacity == 0U) {
        return;
    }

    (void)snprintf(out_message,
                   capacity,
                   "%s",
                   message != NULL ? message : "");
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
        (void)copy_text(command->program,
                        sizeof(command->program),
                        program);
    }
}

void umi_build_command_rebind(UmiBuildCommand *command)
{
    size_t index;

    if (command == NULL) {
        return;
    }

    for (index = 0U; index < UMI_BUILD_MAX_ARGUMENTS; ++index) {
        command->arguments[index] =
            index < command->argument_count
                ? command->argument_storage[index]
                : NULL;
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

UmiStatus umi_build_command_validate(const UmiBuildCommand *command,
                                     char *out_message,
                                     size_t message_capacity)
{
    size_t index;

    if (command == NULL) {
        write_message(out_message, message_capacity, "Command is required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (command->program[0] == '\0') {
        write_message(out_message,
                      message_capacity,
                      "Command program is required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (command->argument_count > UMI_BUILD_MAX_ARGUMENTS) {
        write_message(out_message,
                      message_capacity,
                      "Command argument count exceeds the Framework limit.");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < command->argument_count; ++index) {
        if (memchr(command->argument_storage[index],
                   '\0',
                   sizeof(command->argument_storage[index])) == NULL) {
            write_message(out_message,
                          message_capacity,
                          "Command argument is not terminated.");
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    write_message(out_message, message_capacity, "Command is valid.");
    return UMI_STATUS_OK;
}

static UmiStatus parse_token(const char **cursor,
                             char *out_token,
                             size_t token_capacity,
                             int *out_started,
                             char *out_message,
                             size_t message_capacity)
{
    size_t used = 0U;
    char quote = '\0';
    int started = 0;

    if (cursor == NULL || *cursor == NULL ||
        out_token == NULL || token_capacity == 0U ||
        out_started == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (**cursor != '\0') {
        const char value = **cursor;

        if (quote == '\0' && isspace((unsigned char)value)) {
            break;
        }

        started = 1;

        if (value == '\\') {
            const char next = (*cursor)[1];

            /*
             * Preserve ordinary Windows path separators such as C:\work\src.
             * Backslash acts as an escape only when it has something useful to
             * escape in this deterministic grammar.
             */
            if (next == '\'' || next == '"' || next == '\\' ||
                isspace((unsigned char)next)) {
                if (used + 1U >= token_capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }

                out_token[used++] = next;
                *cursor += 2;
                continue;
            }

            if (used + 1U >= token_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            out_token[used++] = value;
            ++(*cursor);
            continue;
        }

        if (value == '\'' || value == '"') {
            if (quote == '\0') {
                quote = value;
                ++(*cursor);
                continue;
            }

            if (quote == value) {
                quote = '\0';
                ++(*cursor);
                continue;
            }
        }

        if (used + 1U >= token_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        out_token[used++] = value;
        ++(*cursor);
    }

    if (quote != '\0') {
        write_message(out_message,
                      message_capacity,
                      "Command contains an unterminated quote.");
        return UMI_STATUS_PARSE_ERROR;
    }

    out_token[used] = '\0';
    *out_started = started;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_command_parse(const char *text,
                                  UmiBuildCommand *out_command,
                                  char *out_message,
                                  size_t message_capacity)
{
    const char *cursor;
    size_t token_index = 0U;
    UmiStatus status;

    if (text == NULL || out_command == NULL) {
        write_message(out_message,
                      message_capacity,
                      "Command text and output command are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_build_command_init(out_command, NULL);
    cursor = text;

    while (*cursor != '\0') {
        char token[UMI_BUILD_PATH_CAPACITY];
        int started = 0;

        while (isspace((unsigned char)*cursor)) {
            ++cursor;
        }

        if (*cursor == '\0') {
            break;
        }

        status = parse_token(&cursor,
                             token,
                             sizeof(token),
                             &started,
                             out_message,
                             message_capacity);
        if (status != UMI_STATUS_OK) {
            return status;
        }

        if (!started) {
            break;
        }

        if (token_index == 0U) {
            if (token[0] == '\0') {
                write_message(out_message,
                              message_capacity,
                              "Command program cannot be empty.");
                return UMI_STATUS_PARSE_ERROR;
            }

            if (!copy_text(out_command->program,
                           sizeof(out_command->program),
                           token)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        } else {
            if (strlen(token) >= UMI_BUILD_ARGUMENT_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            if (!umi_build_command_add_argument(out_command, token)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }

        ++token_index;
    }

    if (token_index == 0U) {
        write_message(out_message,
                      message_capacity,
                      "Command text is empty.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_build_command_validate(out_command,
                                        out_message,
                                        message_capacity);
    if (status == UMI_STATUS_OK) {
        write_message(out_message,
                      message_capacity,
                      "Command parsed without shell expansion.");
    }

    return status;
}

static int needs_display_quotes(const char *text)
{
    const char *cursor = text;

    if (text == NULL || text[0] == '\0') {
        return 1;
    }

    while (*cursor != '\0') {
        if (isspace((unsigned char)*cursor) ||
            *cursor == '"' || *cursor == '\\') {
            return 1;
        }
        ++cursor;
    }

    return 0;
}

static int append_character(char *out_text,
                            size_t capacity,
                            size_t *used,
                            char value)
{
    if (out_text == NULL || used == NULL || *used + 1U >= capacity) {
        return 0;
    }

    out_text[(*used)++] = value;
    out_text[*used] = '\0';
    return 1;
}

static int append_display_token(char *out_text,
                                size_t capacity,
                                size_t *used,
                                const char *text)
{
    const char *cursor;
    const int quoted = needs_display_quotes(text);

    if (quoted && !append_character(out_text, capacity, used, '"')) {
        return 0;
    }

    for (cursor = text; *cursor != '\0'; ++cursor) {
        if ((*cursor == '"' || *cursor == '\\') &&
            !append_character(out_text, capacity, used, '\\')) {
            return 0;
        }

        if (!append_character(out_text, capacity, used, *cursor)) {
            return 0;
        }
    }

    if (quoted && !append_character(out_text, capacity, used, '"')) {
        return 0;
    }

    return 1;
}

int umi_build_command_format(const UmiBuildCommand *command,
                             char *out_text,
                             size_t capacity)
{
    size_t index;
    size_t used = 0U;

    if (command == NULL || out_text == NULL || capacity == 0U ||
        command->program[0] == '\0') {
        return 0;
    }

    out_text[0] = '\0';

    if (!append_display_token(out_text,
                              capacity,
                              &used,
                              command->program)) {
        return 0;
    }

    for (index = 0U; index < command->argument_count; ++index) {
        if (!append_character(out_text, capacity, &used, ' ') ||
            !append_display_token(out_text,
                                  capacity,
                                  &used,
                                  command->argument_storage[index])) {
            return 0;
        }
    }

    return 1;
}
