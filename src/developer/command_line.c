/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/command_line.c
 *
 * PURPOSE:
 *   Implement conservative command-line tokenisation for legacy project tasks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/command_line.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the is unsafe shell character operation used by this module and its client
 * applications.
 */
static int is_unsafe_shell_character(char value)
{
    return value == '|' || value == '&' || value == ';' || value == '<' ||
           value == '>' || value == '(' || value == ')' || value == '`' ||
           value == '\n' || value == '\r';
}

/* Provide the store token operation used by this module and its client applications. */
static UmiStatus store_token(
    UmiDeveloperCommandLine *command,
    size_t token_index,
    const char *token,
    size_t length)
{
    char *destination;
    size_t capacity;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_index == 0U) {
        destination = command->program;
        capacity = sizeof(command->program);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token_index > UMI_DEVELOPER_MAX_ARGUMENTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        destination = command->arguments[token_index - 1U];
        capacity = sizeof(command->arguments[token_index - 1U]);
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        memcpy(destination, token, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Read developer command line into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_developer_command_line_parse(
    const char *text,
    UmiDeveloperCommandLine *out_command)
{
    char token[UMI_DEVELOPER_PATH_CAPACITY];
    size_t token_length = 0U;
    size_t token_index = 0U;
    size_t index = 0U;
    int quote = 0;
    int escaping = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(out_command, 0, sizeof(*out_command));
    out_command->struct_size = (uint32_t)sizeof(*out_command);
    out_command->api_version = UMI_DEVELOPER_COMMAND_LINE_API_VERSION;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text[index] != '\0') {
        unsigned char current = (unsigned char)text[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (escaping != 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (token_length + 1U >= sizeof(token)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            token[token_length++] = (char)current;
            escaping = 0;
            index += 1U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (current == (unsigned char)'\\') {
            unsigned char next = (unsigned char)text[index + 1U];
            /* Apply this branch only when its contract condition is satisfied. */
            if (next == (unsigned char)'\0') {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (token_length + 1U >= sizeof(token)) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                token[token_length++] = '\\';
                index += 1U;
                continue;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (next == (unsigned char)'\\' || next == (unsigned char)'\"' ||
                next == (unsigned char)'\'' || isspace(next) != 0) {
                escaping = 1;
                index += 1U;
                continue;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (token_length + 1U >= sizeof(token)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            token[token_length++] = '\\';
            index += 1U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (quote != 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if ((quote == 1 && current == (unsigned char)'\'') ||
                (quote == 2 && current == (unsigned char)'\"')) {
                quote = 0;
                index += 1U;
                continue;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (token_length + 1U >= sizeof(token)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            token[token_length++] = (char)current;
            index += 1U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (current == (unsigned char)'\'') {
            quote = 1;
            index += 1U;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (current == (unsigned char)'\"') {
            quote = 2;
            index += 1U;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (is_unsafe_shell_character((char)current) != 0) {
            return UMI_STATUS_PARSE_ERROR;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (isspace(current) != 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (token_length > 0U) {
                status = store_token(out_command, token_index, token, token_length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    return status;
                }
                token_index += 1U;
                token_length = 0U;
            }
            index += 1U;
            continue;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (token_length + 1U >= sizeof(token)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        token[token_length++] = (char)current;
        index += 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (escaping != 0 || quote != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_length > 0U) {
        status = store_token(out_command, token_index, token, token_length);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        token_index += 1U;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_index == 0U || out_command->program[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_command->argument_count = token_index - 1U;
    return UMI_STATUS_OK;
}
