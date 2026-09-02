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

/*
 * Initialise terminal command from caller-provided values so later operations receive a
 * known state.
 */
void umi_terminal_command_init(UmiTerminalCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command != NULL) {
        (void)memset(command, 0, sizeof(*command));
    }
}

/*
 * Read terminal command into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_terminal_command_parse(UmiTerminalCommand *command,
                                     const char *text)
{
    const char *cursor;
    size_t count = 0U;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL || text == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_terminal_command_init(command);
    /* Apply this branch only when its contract condition is satisfied. */
    if (strlen(text) + 1U > sizeof(command->source)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(command->source, text, strlen(text) + 1U);
    cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        char quote = '\0';
        size_t length = 0U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (isspace((unsigned char)*cursor)) {
            cursor += 1;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\0') {
            break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count >= UMI_TERMINAL_MAX_ARGUMENTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"' || *cursor == '\'') {
            quote = *cursor++;
        }
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0' &&
               ((quote != '\0' && *cursor != quote) ||
                (quote == '\0' && !isspace((unsigned char)*cursor)))) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (*cursor == '\\' && cursor[1] != '\0') {
                cursor += 1;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length + 1U >= UMI_TERMINAL_ARGUMENT_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            command->argument_storage[count][length++] = *cursor++;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (quote != '\0' && *cursor != quote) {
            return UMI_STATUS_PARSE_ERROR;
        }
        /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the terminal command program operation used by this module and its client
 * applications.
 */
const char *umi_terminal_command_program(const UmiTerminalCommand *command)
{
    return command != NULL && command->argument_count > 0U
        ? command->arguments[0]
        : NULL;
}

/*
 * Provide the terminal command format operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_command_format(const UmiTerminalCommand *command,
                                      char *out_text,
                                      size_t capacity)
{
    size_t index;
    size_t used = 0U;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_text[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < command->argument_count; ++index) {
        int written = snprintf(out_text + used,
                               capacity - used,
                               "%s\"%s\"",
                               index == 0U ? "" : " ",
                               command->arguments[index]);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}
