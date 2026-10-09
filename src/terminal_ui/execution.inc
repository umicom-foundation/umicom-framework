/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/execution.inc
 * PURPOSE: Capture terminal job identity and publish its history on the controller owner thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiTerminalControllerArmBackground(UmiTerminalController *controller)
{
    if (controller == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->background_armed || controller->execution.execution.pending)
        return UMI_STATUS_BUSY;
    controller->background_armed = 1;
    return UMI_STATUS_OK;
}
int UmiTerminalControllerBackgroundArmed(const UmiTerminalController *controller)
{
    return controller != NULL && controller->background_armed;
}
void UmiTerminalControllerDisarmBackground(UmiTerminalController *controller)
{
    if (controller != NULL)
        controller->background_armed = 0;
}
int UmiTerminalControllerJobPending(const UmiTerminalController *controller)
{
    return controller != NULL && controller->execution.execution.pending;
}
static UmiStatus TerminalControllerStartJob(UmiTerminalController *controller, const char *command,
                                            uint32_t timeout_ms)
{
    if (strlen(command) >= UMI_TERMINAL_COMMAND_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTerminalSession *session = umi_terminal_controller_active_session(controller);
    if (session == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiTerminalSessionSnapshot snapshot;
    UmiStatus status = umi_terminal_session_snapshot(session, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTerminalTabSnapshot tab;
    UmiTerminalProfile profile;
    UmiTerminalCommand prepared;
    status = umi_terminal_tab_model_find(controller->tabs, snapshot.session_id, &tab);
    if (status == UMI_STATUS_OK)
        status = umi_terminal_profile_registry_find(controller->profiles, tab.profile_id, &profile);
    if (status == UMI_STATUS_OK)
        status = prepare_profile_command(&profile, command, &prepared);
    if (status != UMI_STATUS_OK)
        return status;
    UmiTerminalControllerExecution next = {0};
    memcpy(next.session_id, snapshot.session_id, strlen(snapshot.session_id) + 1U);
    memcpy(next.command, command, strlen(command) + 1U);
    char base[UMI_TERMINAL_PATH_CAPACITY] = {0};
    if (!umi_path_is_absolute(snapshot.working_directory))
    {
        status = umi_fs_current_directory(base, sizeof base);
        if (status != UMI_STATUS_OK)
            return status;
    }
    status = umi_path_absolute(snapshot.working_directory, base, next.working_directory,
                               sizeof next.working_directory);
    if (status != UMI_STATUS_OK)
        return status;
    status = UmiTerminalSessionStartJob(session, controller->process_supervisor, &prepared, command,
                                        timeout_ms);
    if (status != UMI_STATUS_OK)
        return status;
    /* Start succeeded. Preserve this association even if a later Poll fails;
     * teardown can still find and cancel the exact originating session. */
    next.execution.pending = 1;
    controller->execution = next;
    emit_event(controller, UMI_TERMINAL_EVENT_COMMAND_STARTED, snapshot.session_id, command, 0);
    publish_output(controller, "Terminal", UMI_OUTPUT_STREAM_SYSTEM, command);
    return UMI_STATUS_OK;
}

UmiStatus UmiTerminalControllerPollJob(UmiTerminalController *controller,
                                       UmiTerminalControllerExecution *out_snapshot)
{
    if (controller == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof *out_snapshot);
    if (controller->execution.session_id[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    if (controller->execution.execution.pending)
    {
        UmiTerminalSession *session =
            umi_terminal_manager_find(controller->manager, controller->execution.session_id);
        if (session == NULL)
            return UMI_STATUS_INVALID_STATE;
        UmiTerminalExecutionSnapshot result;
        UmiStatus status = UmiTerminalSessionPollJob(session, &result);
        if (status != UMI_STATUS_OK)
            return status;
        controller->execution.execution = result;
        if (!result.pending)
        {
            /* This transition, rather than timer frequency or selected tab,
             * determines when one history record and completion event are made. */
            UmiTerminalHistoryEntry history = {0};
            memcpy(history.session_id, controller->execution.session_id,
                   strlen(controller->execution.session_id) + 1U);
            memcpy(history.working_directory, controller->execution.working_directory,
                   strlen(controller->execution.working_directory) + 1U);
            memcpy(history.command, controller->execution.command,
                   strlen(controller->execution.command) + 1U);
            history.timestamp_ns = now_ns(controller);
            history.exit_code = result.process.exit_code;
            history.completed = 1;
            (void)umi_terminal_history_append(controller->history, &history);
            UmiStatus outcome =
                result.process.state == UMI_PROCESS_JOB_CANCELLED   ? UMI_STATUS_CANCELLED
                : result.process.state == UMI_PROCESS_JOB_TIMED_OUT ? UMI_STATUS_TIMEOUT
                : result.process.state == UMI_PROCESS_JOB_SUCCEEDED && result.process.exit_code == 0
                    ? UMI_STATUS_OK
                    : UMI_STATUS_IO_ERROR;
            emit_event(controller, UMI_TERMINAL_EVENT_COMMAND_FINISHED, history.session_id,
                       history.command, (int)outcome);
            publish_output(controller, "Terminal",
                           outcome == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD
                                                    : UMI_OUTPUT_STREAM_ERROR,
                           result.process.output);
        }
    }
    *out_snapshot = controller->execution;
    return UMI_STATUS_OK;
}

UmiStatus UmiTerminalControllerStopJob(UmiTerminalController *controller)
{
    if (controller == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->execution.execution.pending)
        return UMI_STATUS_NOT_FOUND;
    UmiTerminalSession *session =
        umi_terminal_manager_find(controller->manager, controller->execution.session_id);
    return session != NULL ? UmiTerminalSessionStopJob(session) : UMI_STATUS_INVALID_STATE;
}
static void TerminalControllerDrainJob(UmiTerminalController *controller)
{
    if (!controller->execution.execution.pending)
        return;
    UmiTerminalSession *session =
        umi_terminal_manager_find(controller->manager, controller->execution.session_id);
    if (session == NULL)
        return;
    (void)UmiTerminalSessionStopJob(session);
    (void)UmiTerminalSessionWaitJob(session, 0U);
    UmiTerminalControllerExecution result;
    (void)UmiTerminalControllerPollJob(controller, &result);
}
