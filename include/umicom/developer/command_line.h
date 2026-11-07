/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/command_line.h
 *
 * PURPOSE:
 *   Parse a bounded developer command line into a program and explicit argument
 *   vector without invoking a command shell or performing shell expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Project task records created before the structured developer runtime store a
 * command in one text field.  This compatibility parser understands whitespace,
 * single quotes, double quotes and backslash escaping only.  Shell operators
 * such as pipes, redirection, semicolons and command substitution are rejected
 * instead of being executed implicitly.
 */
#ifndef UMICOM_DEVELOPER_COMMAND_LINE_H
#define UMICOM_DEVELOPER_COMMAND_LINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_COMMAND_LINE_API_VERSION 1U
#define UMI_DEVELOPER_COMMAND_TOKEN_CAPACITY (UMI_DEVELOPER_MAX_ARGUMENTS + 1U)

/**
 * Represent the developer command line data shared with callers of this public contract.
 */
typedef struct UmiDeveloperCommandLine {
    uint32_t struct_size;
    uint32_t api_version;
    char program[UMI_DEVELOPER_PATH_CAPACITY];
    char arguments[UMI_DEVELOPER_MAX_ARGUMENTS][UMI_DEVELOPER_ARGUMENT_CAPACITY];
    size_t argument_count;
} UmiDeveloperCommandLine;

/**
 * Read developer command line into validated module state and return a status when input
 * cannot be used.
 */
/**
 * Parse exactly text_length readable bytes, excluding any terminator.
 * Embedded NUL bytes and an empty program are rejected. Empty quoted arguments
 * are retained. On every failure out_command is unchanged. Input may point
 * into out_command because publication happens only after successful parsing.
 * The caller keeps text readable for the call and supplies writable output.
 *
 * This is the legacy task grammar, not PowerShell, cmd.exe or POSIX shell:
 * quotes group bytes; backslash escapes another backslash, either quote or
 * whitespace. Unquoted shell operators are rejected. Quoted operators remain
 * literal argument data. No variable, wildcard or command expansion occurs.
 */
UmiStatus umi_developer_command_line_parse_n(
    const char *text,
    size_t text_length,
    UmiDeveloperCommandLine *out_command);

/* NUL-terminated compatibility entry point; output is unchanged on failure. */
UmiStatus umi_developer_command_line_parse(
    const char *text,
    UmiDeveloperCommandLine *out_command);

#ifdef __cplusplus
}
#endif

#endif
