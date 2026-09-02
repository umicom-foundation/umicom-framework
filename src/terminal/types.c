/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/types.c
 *
 * PURPOSE:
 *   Implement stable terminal state and stream text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/types.h"

/*
 * Provide the terminal state text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_state_text(UmiTerminalState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TERMINAL_CREATED: return "created";
        case UMI_TERMINAL_READY: return "ready";
        case UMI_TERMINAL_RUNNING: return "running";
        case UMI_TERMINAL_CLOSED: return "closed";
        case UMI_TERMINAL_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the terminal stream text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_stream_text(UmiTerminalStream stream)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stream) {
        case UMI_TERMINAL_STREAM_INPUT: return "input";
        case UMI_TERMINAL_STREAM_OUTPUT: return "output";
        case UMI_TERMINAL_STREAM_ERROR: return "error";
        case UMI_TERMINAL_STREAM_SYSTEM: return "system";
        default: return "unknown";
    }
}

/*
 * Provide the terminal orientation text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_orientation_text(UmiTerminalOrientation orientation)
{
    return orientation == UMI_TERMINAL_ORIENTATION_VERTICAL
        ? "vertical" : "horizontal";
}

/*
 * Provide the terminal profile kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_profile_kind_text(UmiTerminalProfileKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TERMINAL_PROFILE_CUSTOM: return "custom";
        case UMI_TERMINAL_PROFILE_POWERSHELL: return "powershell";
        case UMI_TERMINAL_PROFILE_COMMAND_PROMPT: return "command-prompt";
        case UMI_TERMINAL_PROFILE_BASH: return "bash";
        case UMI_TERMINAL_PROFILE_POSIX_SH: return "posix-sh";
        case UMI_TERMINAL_PROFILE_WSL: return "wsl";
        case UMI_TERMINAL_PROFILE_MSYS2_UCRT64: return "msys2-ucrt64";
        default: return "unknown";
    }
}

/*
 * Provide the terminal event kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_event_kind_text(UmiTerminalEventKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TERMINAL_EVENT_SESSION_OPENED: return "session-opened";
        case UMI_TERMINAL_EVENT_SESSION_ACTIVATED: return "session-activated";
        case UMI_TERMINAL_EVENT_SESSION_CLOSED: return "session-closed";
        case UMI_TERMINAL_EVENT_COMMAND_STARTED: return "command-started";
        case UMI_TERMINAL_EVENT_COMMAND_FINISHED: return "command-finished";
        case UMI_TERMINAL_EVENT_TRANSCRIPT_CLEARED: return "transcript-cleared";
        case UMI_TERMINAL_EVENT_PROFILE_CHANGED: return "profile-changed";
        case UMI_TERMINAL_EVENT_LAYOUT_CHANGED: return "layout-changed";
        case UMI_TERMINAL_EVENT_PROCESS_CHANGED: return "process-changed";
        case UMI_TERMINAL_EVENT_TASK_CHANGED: return "task-changed";
        case UMI_TERMINAL_EVENT_HISTORY_CLEARED: return "history-cleared";
        default: return "unknown";
    }
}
