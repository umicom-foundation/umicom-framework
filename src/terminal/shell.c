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

/* Provide the shell kind text operation used by this module and its client applications. */
const char *umi_shell_kind_text(UmiShellKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_SHELL_POWERSHELL: return "PowerShell";
        case UMI_SHELL_COMMAND_PROMPT: return "Command Prompt";
        case UMI_SHELL_BASH: return "Bash";
        case UMI_SHELL_SH: return "POSIX sh";
        default: return "Unknown";
    }
}

/* Provide the shell detect operation used by this module and its client applications. */
UmiStatus umi_shell_detect(UmiShellDescriptor *out_shell)
{
    const char *shell;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (shell != NULL && strstr(shell, "bash") != NULL) {
        out_shell->kind = UMI_SHELL_BASH;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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

/*
 * Provide the shell create command operation used by this module and its client
 * applications.
 */
UmiStatus umi_shell_create_command(const UmiShellDescriptor *shell,
                                   const char *script,
                                   UmiTerminalCommand *out_command)
{
    char command_text[UMI_TERMINAL_COMMAND_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (shell == NULL || script == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Select the behaviour associated with the requested command or state value. */
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
