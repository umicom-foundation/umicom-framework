/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/session.c
 *
 * PURPOSE:
 *   Execute explicit terminal commands with captured output and retained transcript.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/process.h"
#include "umicom/platform/threading.h"

struct UmiTerminalSession {
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    char working_directory[UMI_TERMINAL_PATH_CAPACITY];
    UmiTerminalState state;
    uint64_t commands_executed;
    int last_exit_code;
    UmiClock *clock;
    UmiTerminalEnvironment *environment;
    UmiTerminalTranscript *transcript;
    UmiMutex *mutex;
};

static uint64_t now_ns(UmiTerminalSession *session)
{
    return session->clock != NULL &&
           session->clock->wall_nanoseconds != NULL
        ? session->clock->wall_nanoseconds(session->clock)
        : 0U;
}

UmiStatus umi_terminal_session_create(
    const UmiTerminalSessionConfig *config,
    UmiTerminalSession **out_session)
{
    UmiTerminalSession *session;
    UmiStatus status;
    if (config == NULL || out_session == NULL ||
        config->session_id == NULL || config->session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiTerminalSession *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(session->session_id,
                   sizeof(session->session_id),
                   "%s",
                   config->session_id);
    (void)snprintf(session->title,
                   sizeof(session->title),
                   "%s",
                   config->title != NULL ? config->title : config->session_id);
    (void)snprintf(session->working_directory,
                   sizeof(session->working_directory),
                   "%s",
                   config->working_directory != NULL
                       ? config->working_directory : ".");
    session->clock = config->clock;
    session->state = UMI_TERMINAL_READY;
    status = umi_mutex_create(&session->mutex);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_environment_create(&session->environment);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_transcript_create(
            config->transcript_capacity > 0U
                ? config->transcript_capacity
                : 256U,
            &session->transcript
        );
    }
    if (status != UMI_STATUS_OK) {
        umi_terminal_session_destroy(session);
        return status;
    }
    (void)umi_terminal_transcript_append(session->transcript,
                                         now_ns(session),
                                         UMI_TERMINAL_STREAM_SYSTEM,
                                         "Terminal session ready");
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_terminal_session_destroy(UmiTerminalSession *session)
{
    if (session != NULL) {
        umi_terminal_transcript_destroy(session->transcript);
        umi_terminal_environment_destroy(session->environment);
        umi_mutex_destroy(session->mutex);
        free(session);
    }
}

UmiStatus umi_terminal_session_set_working_directory(
    UmiTerminalSession *session,
    const char *working_directory)
{
    if (session == NULL || working_directory == NULL ||
        working_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(working_directory) + 1U >
        sizeof(session->working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_mutex_lock(session->mutex);
    (void)snprintf(session->working_directory,
                   sizeof(session->working_directory),
                   "%s",
                   working_directory);
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_session_execute(UmiTerminalSession *session,
                                       const char *command_text,
                                       uint32_t timeout_ms,
                                       UmiCancellationToken *cancellation,
                                       int *out_exit_code)
{
    UmiTerminalCommand command;
    UmiEnvironmentVariable variables[UMI_TERMINAL_MAX_ENVIRONMENT];
    size_t variable_count = 0U;
    UmiProcessRequest request;
    UmiProcessResult result;
    const char *program;
    UmiStatus status;

    if (session == NULL || command_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(session->mutex);
    if (session->state == UMI_TERMINAL_CLOSED) {
        (void)umi_mutex_unlock(session->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    session->state = UMI_TERMINAL_RUNNING;
    (void)umi_mutex_unlock(session->mutex);

    status = umi_terminal_command_parse(&command, command_text);
    program = status == UMI_STATUS_OK
        ? umi_terminal_command_program(&command)
        : NULL;
    if (status != UMI_STATUS_OK || program == NULL) {
        (void)umi_mutex_lock(session->mutex);
        session->state = UMI_TERMINAL_FAILED;
        (void)umi_mutex_unlock(session->mutex);
        return status;
    }

    (void)umi_terminal_transcript_append(session->transcript,
                                         now_ns(session),
                                         UMI_TERMINAL_STREAM_INPUT,
                                         command_text);
    status = umi_terminal_environment_export(session->environment,
                                             variables,
                                             UMI_TERMINAL_MAX_ENVIRONMENT,
                                             &variable_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = command.argument_count > 1U
        ? &command.arguments[1]
        : NULL;
    request.argument_count = command.argument_count > 0U
        ? command.argument_count - 1U
        : 0U;
    request.working_directory = session->working_directory;
    request.environment = variables;
    request.environment_count = variable_count;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = timeout_ms;
    request.cancellation = cancellation;
    status = umi_process_execute(&request, &result);

    (void)umi_terminal_transcript_append(
        session->transcript,
        now_ns(session),
        status == UMI_STATUS_OK
            ? UMI_TERMINAL_STREAM_OUTPUT
            : UMI_TERMINAL_STREAM_ERROR,
        result.output
    );
    (void)umi_mutex_lock(session->mutex);
    session->commands_executed += 1U;
    session->last_exit_code = result.exit_code;
    session->state = status == UMI_STATUS_OK
        ? UMI_TERMINAL_READY
        : UMI_TERMINAL_FAILED;
    (void)umi_mutex_unlock(session->mutex);
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    return status;
}

UmiStatus umi_terminal_session_snapshot(
    const UmiTerminalSession *session,
    UmiTerminalSessionSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(session->mutex);
    (void)snprintf(out_snapshot->session_id,
                   sizeof(out_snapshot->session_id),
                   "%s",
                   session->session_id);
    (void)snprintf(out_snapshot->title,
                   sizeof(out_snapshot->title),
                   "%s",
                   session->title);
    (void)snprintf(out_snapshot->working_directory,
                   sizeof(out_snapshot->working_directory),
                   "%s",
                   session->working_directory);
    out_snapshot->state = session->state;
    out_snapshot->commands_executed = session->commands_executed;
    out_snapshot->last_exit_code = session->last_exit_code;
    out_snapshot->transcript_lines =
        umi_terminal_transcript_count(session->transcript);
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

UmiTerminalEnvironment *umi_terminal_session_environment(
    UmiTerminalSession *session)
{
    return session != NULL ? session->environment : NULL;
}

UmiTerminalTranscript *umi_terminal_session_transcript(
    UmiTerminalSession *session)
{
    return session != NULL ? session->transcript : NULL;
}

UmiStatus umi_terminal_session_close(UmiTerminalSession *session)
{
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(session->mutex);
    session->state = UMI_TERMINAL_CLOSED;
    (void)umi_mutex_unlock(session->mutex);
    (void)umi_terminal_transcript_append(session->transcript,
                                         now_ns(session),
                                         UMI_TERMINAL_STREAM_SYSTEM,
                                         "Terminal session closed");
    return UMI_STATUS_OK;
}
