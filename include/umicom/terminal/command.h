/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/command.h
 *
 * PURPOSE:
 *   Parse an explicit command line into owned program and argument records without invoking an implicit shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_COMMAND_H
#define UMICOM_TERMINAL_COMMAND_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalCommand {
    char source[UMI_TERMINAL_COMMAND_CAPACITY];
    char argument_storage[UMI_TERMINAL_MAX_ARGUMENTS]
                         [UMI_TERMINAL_ARGUMENT_CAPACITY];
    const char *arguments[UMI_TERMINAL_MAX_ARGUMENTS];
    size_t argument_count;
} UmiTerminalCommand;

void umi_terminal_command_init(UmiTerminalCommand *command);
UmiStatus umi_terminal_command_parse(UmiTerminalCommand *command,
                                     const char *text);
const char *umi_terminal_command_program(const UmiTerminalCommand *command);
UmiStatus umi_terminal_command_format(const UmiTerminalCommand *command,
                                      char *out_text,
                                      size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
