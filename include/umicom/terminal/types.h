/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/types.h
 *
 * PURPOSE:
 *   Define terminal session states, stream kinds and fixed limits shared by terminal services and Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
#define UMI_TERMINAL_PROFILE_MAX 32U
#define UMI_TERMINAL_PROFILE_ARGUMENT_MAX 16U
#define UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX 32U
#define UMI_TERMINAL_HISTORY_MAX 1024U
#define UMI_TERMINAL_LINK_MAX 256U
#define UMI_TERMINAL_TAB_MAX 64U
#define UMI_TERMINAL_SPLIT_MAX 64U
#define UMI_TERMINAL_EVENT_MAX 2048U
#define UMI_TERMINAL_QUERY_CAPACITY 512U

/**
 * List the named terminal state values accepted by this public contract.
 */
typedef enum UmiTerminalState {
    UMI_TERMINAL_CREATED = 0,
    UMI_TERMINAL_READY = 1,
    UMI_TERMINAL_RUNNING = 2,
    UMI_TERMINAL_CLOSED = 3,
    UMI_TERMINAL_FAILED = 4
} UmiTerminalState;

/**
 * List the named terminal stream values accepted by this public contract.
 */
typedef enum UmiTerminalStream {
    UMI_TERMINAL_STREAM_INPUT = 0,
    UMI_TERMINAL_STREAM_OUTPUT = 1,
    UMI_TERMINAL_STREAM_ERROR = 2,
    UMI_TERMINAL_STREAM_SYSTEM = 3
} UmiTerminalStream;

/**
 * List the named terminal orientation values accepted by this public contract.
 */
typedef enum UmiTerminalOrientation {
    UMI_TERMINAL_ORIENTATION_HORIZONTAL = 0,
    UMI_TERMINAL_ORIENTATION_VERTICAL = 1
} UmiTerminalOrientation;

/**
 * List the named terminal profile kind values accepted by this public contract.
 */
typedef enum UmiTerminalProfileKind {
    UMI_TERMINAL_PROFILE_CUSTOM = 0,
    UMI_TERMINAL_PROFILE_POWERSHELL = 1,
    UMI_TERMINAL_PROFILE_COMMAND_PROMPT = 2,
    UMI_TERMINAL_PROFILE_BASH = 3,
    UMI_TERMINAL_PROFILE_POSIX_SH = 4,
    UMI_TERMINAL_PROFILE_WSL = 5,
    UMI_TERMINAL_PROFILE_MSYS2_UCRT64 = 6
} UmiTerminalProfileKind;

/**
 * List the named terminal event kind values accepted by this public contract.
 */
typedef enum UmiTerminalEventKind {
    UMI_TERMINAL_EVENT_SESSION_OPENED = 0,
    UMI_TERMINAL_EVENT_SESSION_ACTIVATED = 1,
    UMI_TERMINAL_EVENT_SESSION_CLOSED = 2,
    UMI_TERMINAL_EVENT_COMMAND_STARTED = 3,
    UMI_TERMINAL_EVENT_COMMAND_FINISHED = 4,
    UMI_TERMINAL_EVENT_TRANSCRIPT_CLEARED = 5,
    UMI_TERMINAL_EVENT_PROFILE_CHANGED = 6,
    UMI_TERMINAL_EVENT_LAYOUT_CHANGED = 7,
    UMI_TERMINAL_EVENT_PROCESS_CHANGED = 8,
    UMI_TERMINAL_EVENT_TASK_CHANGED = 9,
    UMI_TERMINAL_EVENT_HISTORY_CLEARED = 10
} UmiTerminalEventKind;

/**
 * Provide the terminal state text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_state_text(UmiTerminalState state);
/**
 * Provide the terminal stream text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_stream_text(UmiTerminalStream stream);
/**
 * Provide the terminal orientation text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_orientation_text(UmiTerminalOrientation orientation);
/**
 * Provide the terminal profile kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_profile_kind_text(UmiTerminalProfileKind kind);
/**
 * Provide the terminal event kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_event_kind_text(UmiTerminalEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
