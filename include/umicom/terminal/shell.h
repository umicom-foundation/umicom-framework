/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/shell.h
 *
 * PURPOSE:
 *   Detect the platform shell and construct prepared shell commands without requiring a PowerShell bootstrap script.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_SHELL_H
#define UMICOM_TERMINAL_SHELL_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/terminal/command.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiShellKind {
    UMI_SHELL_UNKNOWN = 0,
    UMI_SHELL_POWERSHELL = 1,
    UMI_SHELL_COMMAND_PROMPT = 2,
    UMI_SHELL_BASH = 3,
    UMI_SHELL_SH = 4
} UmiShellKind;

typedef struct UmiShellDescriptor {
    UmiShellKind kind;
    char program[UMI_TERMINAL_PATH_CAPACITY];
    char display_name[UMI_TERMINAL_TITLE_CAPACITY];
} UmiShellDescriptor;

UmiStatus umi_shell_detect(UmiShellDescriptor *out_shell);
UmiStatus umi_shell_create_command(const UmiShellDescriptor *shell,
                                   const char *script,
                                   UmiTerminalCommand *out_command);
const char *umi_shell_kind_text(UmiShellKind kind);

#ifdef __cplusplus
}
#endif

#endif
