/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/types.h
 *
 * PURPOSE:
 *   Define terminal session states, stream kinds and fixed limits shared by terminal services and Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_TYPES_H
#define UMICOM_TERMINAL_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TERMINAL_ID_CAPACITY 96U
#define UMI_TERMINAL_TITLE_CAPACITY 160U
#define UMI_TERMINAL_PATH_CAPACITY 2048U
#define UMI_TERMINAL_COMMAND_CAPACITY 4096U
#define UMI_TERMINAL_ARGUMENT_CAPACITY 512U
#define UMI_TERMINAL_MAX_ARGUMENTS 64U
#define UMI_TERMINAL_MAX_ENVIRONMENT 64U
#define UMI_TERMINAL_TRANSCRIPT_MAX 1024U
#define UMI_TERMINAL_LINE_CAPACITY 2048U
#define UMI_TERMINAL_MANAGER_MAX_SESSIONS 64U

typedef enum UmiTerminalState {
    UMI_TERMINAL_CREATED = 0,
    UMI_TERMINAL_READY = 1,
    UMI_TERMINAL_RUNNING = 2,
    UMI_TERMINAL_CLOSED = 3,
    UMI_TERMINAL_FAILED = 4
} UmiTerminalState;

typedef enum UmiTerminalStream {
    UMI_TERMINAL_STREAM_INPUT = 0,
    UMI_TERMINAL_STREAM_OUTPUT = 1,
    UMI_TERMINAL_STREAM_ERROR = 2,
    UMI_TERMINAL_STREAM_SYSTEM = 3
} UmiTerminalStream;

const char *umi_terminal_state_text(UmiTerminalState state);
const char *umi_terminal_stream_text(UmiTerminalStream stream);

#ifdef __cplusplus
}
#endif

#endif
