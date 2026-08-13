/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/session.h
 *
 * PURPOSE:
 *   Own one terminal session, its working directory, environment, transcript and command execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_SESSION_H
#define UMICOM_TERMINAL_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/clock.h"
#include "umicom/terminal/command.h"
#include "umicom/terminal/environment.h"
#include "umicom/terminal/transcript.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalSession UmiTerminalSession;

typedef struct UmiTerminalSessionConfig {
    const char *session_id;
    const char *title;
    const char *working_directory;
    size_t transcript_capacity;
    UmiClock *clock;
} UmiTerminalSessionConfig;

typedef struct UmiTerminalSessionSnapshot {
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    char working_directory[UMI_TERMINAL_PATH_CAPACITY];
    UmiTerminalState state;
    uint64_t commands_executed;
    int last_exit_code;
    size_t transcript_lines;
} UmiTerminalSessionSnapshot;

UmiStatus umi_terminal_session_create(
    const UmiTerminalSessionConfig *config,
    UmiTerminalSession **out_session
);
void umi_terminal_session_destroy(UmiTerminalSession *session);
UmiStatus umi_terminal_session_set_working_directory(
    UmiTerminalSession *session,
    const char *working_directory
);
UmiStatus umi_terminal_session_execute(UmiTerminalSession *session,
                                       const char *command_text,
                                       uint32_t timeout_ms,
                                       UmiCancellationToken *cancellation,
                                       int *out_exit_code);
UmiStatus umi_terminal_session_execute_prepared(
    UmiTerminalSession *session,
    const UmiTerminalCommand *command,
    const char *display_text,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code);
UmiStatus umi_terminal_session_snapshot(
    const UmiTerminalSession *session,
    UmiTerminalSessionSnapshot *out_snapshot
);
UmiTerminalEnvironment *umi_terminal_session_environment(
    UmiTerminalSession *session
);
UmiTerminalTranscript *umi_terminal_session_transcript(
    UmiTerminalSession *session
);
UmiStatus umi_terminal_session_close(UmiTerminalSession *session);

#ifdef __cplusplus
}
#endif

#endif
