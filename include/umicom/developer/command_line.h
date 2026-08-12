/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/command_line.h
 *
 * PURPOSE:
 *   Parse a bounded developer command line into a program and explicit argument
 *   vector without invoking a command shell or performing shell expansion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiDeveloperCommandLine {
    uint32_t struct_size;
    uint32_t api_version;
    char program[UMI_DEVELOPER_PATH_CAPACITY];
    char arguments[UMI_DEVELOPER_MAX_ARGUMENTS][UMI_DEVELOPER_ARGUMENT_CAPACITY];
    size_t argument_count;
} UmiDeveloperCommandLine;

UmiStatus umi_developer_command_line_parse(
    const char *text,
    UmiDeveloperCommandLine *out_command);

#ifdef __cplusplus
}
#endif

#endif
