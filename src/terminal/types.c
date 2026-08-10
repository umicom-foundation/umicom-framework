/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/types.c
 *
 * PURPOSE:
 *   Implement stable terminal state and stream text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/types.h"

const char *umi_terminal_state_text(UmiTerminalState state)
{
    switch (state) {
        case UMI_TERMINAL_CREATED: return "created";
        case UMI_TERMINAL_READY: return "ready";
        case UMI_TERMINAL_RUNNING: return "running";
        case UMI_TERMINAL_CLOSED: return "closed";
        case UMI_TERMINAL_FAILED: return "failed";
        default: return "unknown";
    }
}

const char *umi_terminal_stream_text(UmiTerminalStream stream)
{
    switch (stream) {
        case UMI_TERMINAL_STREAM_INPUT: return "input";
        case UMI_TERMINAL_STREAM_OUTPUT: return "output";
        case UMI_TERMINAL_STREAM_ERROR: return "error";
        case UMI_TERMINAL_STREAM_SYSTEM: return "system";
        default: return "unknown";
    }
}
