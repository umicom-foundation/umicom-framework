/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/execution.inc
 * PURPOSE: Publish supervised command results on the terminal session owning thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Copy text in bounded records without silently losing the end of a long line.
 * Prefer newline boundaries and keep complete UTF-8 characters together when
 * the bytes are valid. Rendering may replace invalid bytes for display. */
static void TerminalAppendCaptured(UmiTerminalSession *session, UmiTerminalStream stream,
                                   const char *text)
{
    while (text[0] != '\0')
    {
        size_t length = strlen(text);
        size_t count =
            length < UMI_TERMINAL_LINE_CAPACITY ? length : UMI_TERMINAL_LINE_CAPACITY - 1U;
        const char *newline = memchr(text, '\n', count);
        if (newline != NULL)
            count = (size_t)(newline - text) + 1U;
        else if (count < length)
        {
            size_t boundary = count;
            while (boundary > 0U && ((unsigned char)text[boundary] & 0xc0U) == 0x80U)
                --boundary;
            if (boundary != 0U)
                count = boundary;
        }
        char line[UMI_TERMINAL_LINE_CAPACITY];
        memcpy(line, text, count);
        line[count] = '\0';
        (void)umi_terminal_transcript_append(session->transcript, now_ns(session), stream, line);
        text += count;
    }
}

UmiStatus UmiTerminalSessionStartJob(UmiTerminalSession *session, UmiProcessSupervisor *supervisor,
                                     const UmiTerminalCommand *command, const char *display_text,
                                     uint32_t timeout_ms)
{
    if (session == NULL || supervisor == NULL || command == NULL || display_text == NULL ||
        display_text[0] == '\0' || strlen(display_text) >= UMI_TERMINAL_COMMAND_CAPACITY ||
        command->argument_count == 0U || command->argument_count > UMI_TERMINAL_MAX_ARGUMENTS)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *program = umi_terminal_command_program(command);
    if (program == NULL || program[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiTerminalSessionSnapshot before;
    UmiStatus status = umi_terminal_session_snapshot(session, &before);
    if (status != UMI_STATUS_OK)
        return status;
    if (before.state == UMI_TERMINAL_CLOSED)
        return UMI_STATUS_INVALID_STATE;
    if (before.state == UMI_TERMINAL_RUNNING || session->job_id != 0U)
        return UMI_STATUS_BUSY;
    /* Resolve the directory before submission so another process cannot change
     * the interpretation of a relative session path while the worker starts. */
    char directory[UMI_TERMINAL_PATH_CAPACITY];
    char base[UMI_TERMINAL_PATH_CAPACITY] = {0};
    if (!umi_path_is_absolute(before.working_directory))
    {
        status = umi_fs_current_directory(base, sizeof base);
        if (status != UMI_STATUS_OK)
            return status;
    }
    status = umi_path_absolute(before.working_directory, base, directory, sizeof directory);
    if (status != UMI_STATUS_OK)
        return status;
    UmiEnvironmentVariable variables[UMI_TERMINAL_MAX_ENVIRONMENT];
    size_t variable_count = 0U;
    status = umi_terminal_environment_export(session->environment, variables,
                                             UMI_TERMINAL_MAX_ENVIRONMENT, &variable_count);
    if (status != UMI_STATUS_OK)
        return status;
    UmiProcessRequest request = {0};
    request.program = program;
    request.arguments = command->argument_count > 1U ? &command->arguments[1] : NULL;
    request.argument_count = command->argument_count - 1U;
    request.working_directory = directory;
    request.environment = variables;
    request.environment_count = variable_count;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = timeout_ms;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiProcessJobId job = 0U;
    status = UmiProcessSupervisorSubmitWithLifetime(supervisor, before.title, &request,
                                                    UMI_PROCESS_LIFETIME_TREE, &job);
    if (status != UMI_STATUS_OK)
        return status;
    /* The supervisor has copied all borrowed pointers. No worker references the
     * session, environment model, parsed command, or UI entry after this point. */
    session->job_supervisor = supervisor;
    session->job_id = job;
    memset(&session->execution, 0, sizeof session->execution);
    session->execution.process.job_id = job;
    session->execution.process.state = UMI_PROCESS_JOB_CREATED;
    session->execution.process.exit_code = -1;
    session->execution.pending = 1;
    (void)umi_mutex_lock(session->mutex);
    session->state = UMI_TERMINAL_RUNNING;
    (void)umi_mutex_unlock(session->mutex);
    TerminalAppendCaptured(session, UMI_TERMINAL_STREAM_INPUT, display_text);
    return UMI_STATUS_OK;
}

UmiStatus UmiTerminalSessionPollJob(UmiTerminalSession *session,
                                    UmiTerminalExecutionSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof *out_snapshot);
    if (session->execution.process.job_id == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (session->job_id != 0U)
    {
        UmiProcessJobSnapshot snapshot;
        UmiStatus status =
            umi_process_supervisor_snapshot(session->job_supervisor, session->job_id, &snapshot);
        if (status != UMI_STATUS_OK)
            return status;
        session->execution.process = snapshot;
        if (snapshot.state != UMI_PROCESS_JOB_CREATED && snapshot.state != UMI_PROCESS_JOB_RUNNING)
        {
            /* Release before recording completion. A retry after a busy release
             * cannot duplicate history, transcript entries, or command counters. */
            status = UmiProcessSupervisorReleaseJob(session->job_supervisor, session->job_id);
            if (status != UMI_STATUS_OK)
                return status;
            session->job_id = 0U;
            session->job_supervisor = NULL;
            session->execution.pending = 0;
            if (snapshot.output_truncated)
                TerminalAppendCaptured(session, UMI_TERMINAL_STREAM_SYSTEM,
                                       "Only the most recent captured output is retained; earlier "
                                       "output was truncated.");
            TerminalAppendCaptured(session,
                                   snapshot.state == UMI_PROCESS_JOB_SUCCEEDED
                                       ? UMI_TERMINAL_STREAM_OUTPUT
                                       : UMI_TERMINAL_STREAM_ERROR,
                                   snapshot.output);
            char result[160];
            (void)snprintf(result, sizeof result, "Command %s; exit code %d",
                           snapshot.state == UMI_PROCESS_JOB_SUCCEEDED   ? "finished"
                           : snapshot.state == UMI_PROCESS_JOB_CANCELLED ? "stopped"
                           : snapshot.state == UMI_PROCESS_JOB_TIMED_OUT ? "timed out"
                                                                         : "failed",
                           snapshot.exit_code);
            TerminalAppendCaptured(session, UMI_TERMINAL_STREAM_SYSTEM, result);
            (void)umi_mutex_lock(session->mutex);
            ++session->commands_executed;
            session->last_exit_code = snapshot.exit_code;
            session->state = snapshot.state == UMI_PROCESS_JOB_SUCCEEDED && snapshot.exit_code == 0
                                 ? UMI_TERMINAL_READY
                                 : UMI_TERMINAL_FAILED;
            (void)umi_mutex_unlock(session->mutex);
        }
    }
    *out_snapshot = session->execution;
    return UMI_STATUS_OK;
}

UmiStatus UmiTerminalSessionStopJob(UmiTerminalSession *session)
{
    if (session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->job_id == 0U)
        return UMI_STATUS_NOT_FOUND;
    UmiStatus status = umi_process_supervisor_cancel(session->job_supervisor, session->job_id);
    if (status == UMI_STATUS_OK)
        session->execution.stop_requested = 1;
    return status;
}

UmiStatus UmiTerminalSessionWaitJob(UmiTerminalSession *session, uint32_t timeout_ms)
{
    if (session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->job_id != 0U)
    {
        UmiStatus status =
            umi_process_supervisor_wait(session->job_supervisor, session->job_id, timeout_ms);
        if (status != UMI_STATUS_OK)
            return status;
    }
    UmiTerminalExecutionSnapshot snapshot;
    return UmiTerminalSessionPollJob(session, &snapshot);
}
