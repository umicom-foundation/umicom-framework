/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/shell.c
 *
 * PURPOSE:
 *   Implement platform shell detection and explicit script command construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *umi_shell_kind_text(UmiShellKind kind)
{
    switch (kind) {
        case UMI_SHELL_POWERSHELL: return "PowerShell";
        case UMI_SHELL_COMMAND_PROMPT: return "Command Prompt";
        case UMI_SHELL_BASH: return "Bash";
        case UMI_SHELL_SH: return "POSIX sh";
        default: return "Unknown";
    }
}

UmiStatus umi_shell_detect(UmiShellDescriptor *out_shell)
{
    const char *shell;
    if (out_shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_shell, 0, sizeof(*out_shell));
#ifdef _WIN32
    shell = getenv("COMSPEC");
    out_shell->kind = UMI_SHELL_COMMAND_PROMPT;
    (void)snprintf(out_shell->program,
                   sizeof(out_shell->program),
                   "%s",
                   shell != NULL ? shell : "cmd.exe");
#else
    shell = getenv("SHELL");
    if (shell != NULL && strstr(shell, "bash") != NULL) {
        out_shell->kind = UMI_SHELL_BASH;
    } else {
        out_shell->kind = UMI_SHELL_SH;
    }
    (void)snprintf(out_shell->program,
                   sizeof(out_shell->program),
                   "%s",
                   shell != NULL ? shell : "/bin/sh");
#endif
    (void)snprintf(out_shell->display_name,
                   sizeof(out_shell->display_name),
                   "%s",
                   umi_shell_kind_text(out_shell->kind));
    return UMI_STATUS_OK;
}

UmiStatus umi_shell_create_command(const UmiShellDescriptor *shell,
                                   const char *script,
                                   UmiTerminalCommand *out_command)
{
    char command_text[UMI_TERMINAL_COMMAND_CAPACITY];
    if (shell == NULL || script == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    switch (shell->kind) {
        case UMI_SHELL_COMMAND_PROMPT:
            (void)snprintf(command_text,
                           sizeof(command_text),
                           "\"%s\" /D /S /C \"%s\"",
                           shell->program,
                           script);
            break;
        case UMI_SHELL_POWERSHELL:
            (void)snprintf(command_text,
                           sizeof(command_text),
                           "\"%s\" -NoProfile -Command \"%s\"",
                           shell->program,
                           script);
            break;
        case UMI_SHELL_BASH:
        case UMI_SHELL_SH:
            (void)snprintf(command_text,
                           sizeof(command_text),
                           "\"%s\" -c \"%s\"",
                           shell->program,
                           script);
            break;
        default:
            return UMI_STATUS_NOT_FOUND;
    }
    return umi_terminal_command_parse(out_command, command_text);
}
