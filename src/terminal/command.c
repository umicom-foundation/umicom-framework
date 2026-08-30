/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/command.c
 *
 * PURPOSE:
 *   Implement quoted argument parsing and canonical command formatting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/command.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void umi_terminal_command_init(UmiTerminalCommand *command)
{
    if (command != NULL) {
        (void)memset(command, 0, sizeof(*command));
    }
}

UmiStatus umi_terminal_command_parse(UmiTerminalCommand *command,
                                     const char *text)
{
    const char *cursor;
    size_t count = 0U;
    if (command == NULL || text == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_terminal_command_init(command);
    if (strlen(text) + 1U > sizeof(command->source)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(command->source, text, strlen(text) + 1U);
    cursor = text;
    while (*cursor != '\0') {
        char quote = '\0';
        size_t length = 0U;
        while (isspace((unsigned char)*cursor)) {
            cursor += 1;
        }
        if (*cursor == '\0') {
            break;
        }
        if (count >= UMI_TERMINAL_MAX_ARGUMENTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (*cursor == '"' || *cursor == '\'') {
            quote = *cursor++;
        }
        while (*cursor != '\0' &&
               ((quote != '\0' && *cursor != quote) ||
                (quote == '\0' && !isspace((unsigned char)*cursor)))) {
            if (*cursor == '\\' && cursor[1] != '\0') {
                cursor += 1;
            }
            if (length + 1U >= UMI_TERMINAL_ARGUMENT_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            command->argument_storage[count][length++] = *cursor++;
        }
        if (quote != '\0' && *cursor != quote) {
            return UMI_STATUS_PARSE_ERROR;
        }
        if (quote != '\0') {
            cursor += 1;
        }
        command->argument_storage[count][length] = '\0';
        command->arguments[count] = command->argument_storage[count];
        count += 1U;
    }
    command->argument_count = count;
    return count > 0U ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

const char *umi_terminal_command_program(const UmiTerminalCommand *command)
{
    return command != NULL && command->argument_count > 0U
        ? command->arguments[0]
        : NULL;
}

UmiStatus umi_terminal_command_format(const UmiTerminalCommand *command,
                                      char *out_text,
                                      size_t capacity)
{
    size_t index;
    size_t used = 0U;
    if (command == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_text[0] = '\0';
    for (index = 0U; index < command->argument_count; ++index) {
        int written = snprintf(out_text + used,
                               capacity - used,
                               "%s\"%s\"",
                               index == 0U ? "" : " ",
                               command->arguments[index]);
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}
