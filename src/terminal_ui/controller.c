/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/controller.c
 *
 * PURPOSE:
 *   Coordinate borrowed runtime services with owned terminal presentation
 *   models. The controller never owns the application service container.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/terminal/shell.h"

struct UmiTerminalController {
    UmiTerminalManager *manager;
    UmiProcessSupervisor *process_supervisor;
    UmiTaskQueue *task_queue;
    UmiDiagnosticPipeline *pipeline;
    UmiBuildGraph *operation_graph;
    UmiClock *clock;
    UmiTerminalProfileRegistry *profiles;
    UmiTerminalHistory *history;
    UmiTerminalTabModel *tabs;
    UmiTerminalSplitModel *splits;
    UmiTerminalEventLog *events;
    uint64_t revision;
};

static uint64_t now_ns(const UmiTerminalController *controller)
{
    return controller != NULL && controller->clock != NULL &&
           controller->clock->wall_nanoseconds != NULL
        ? controller->clock->wall_nanoseconds(controller->clock) : 0U;
}

static void emit_event(UmiTerminalController *controller,
                       UmiTerminalEventKind kind,
                       const char *subject_id,
                       const char *message,
                       int status_code)
{
    UmiTerminalEvent event = {0};
    if (controller == NULL) return;
    event.timestamp_ns = now_ns(controller);
    event.kind = kind;
    event.status_code = status_code;
    (void)snprintf(event.subject_id, sizeof(event.subject_id), "%s",
                   subject_id != NULL ? subject_id : "");
    (void)snprintf(event.message, sizeof(event.message), "%s",
                   message != NULL ? message : "");
    (void)umi_terminal_event_log_append(controller->events, &event);
    controller->revision += 1U;
}

static void publish_output(UmiTerminalController *controller,
                           const char *channel,
                           UmiOutputStream stream,
                           const char *text)
{
    if (controller != NULL && controller->pipeline != NULL && text != NULL) {
        (void)umi_diagnostic_pipeline_ingest_line(controller->pipeline,
                                                  "terminal",
                                                  channel,
                                                  "terminal",
                                                  stream,
                                                  text,
                                                  now_ns(controller));
    }
}

static UmiStatus prepare_profile_command(const UmiTerminalProfile *profile,
                                         const char *command_text,
                                         UmiTerminalCommand *out_command)
{
    UmiShellDescriptor shell = {0};
    char prepared[UMI_TERMINAL_COMMAND_CAPACITY];
    int written;
    if (profile == NULL || command_text == NULL || out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(shell.program, sizeof(shell.program), "%s", profile->program);
    switch (profile->kind) {
        case UMI_TERMINAL_PROFILE_POWERSHELL:
            shell.kind = UMI_SHELL_POWERSHELL;
            break;
        case UMI_TERMINAL_PROFILE_COMMAND_PROMPT:
            shell.kind = UMI_SHELL_COMMAND_PROMPT;
            break;
        case UMI_TERMINAL_PROFILE_BASH:
        case UMI_TERMINAL_PROFILE_MSYS2_UCRT64:
            shell.kind = UMI_SHELL_BASH;
            break;
        case UMI_TERMINAL_PROFILE_POSIX_SH:
            shell.kind = UMI_SHELL_SH;
            break;
        case UMI_TERMINAL_PROFILE_WSL:
            written = snprintf(prepared, sizeof(prepared),
                               "\"%s\" --exec sh -lc \"%s\"",
                               profile->program, command_text);
            return written >= 0 && (size_t)written < sizeof(prepared)
                ? umi_terminal_command_parse(out_command, prepared)
                : UMI_STATUS_CAPACITY_EXCEEDED;
        case UMI_TERMINAL_PROFILE_CUSTOM:
        default:
            return umi_terminal_command_parse(out_command, command_text);
    }
    return umi_shell_create_command(&shell, command_text, out_command);
}

UmiTerminalControllerConfig umi_terminal_controller_config_default(void)
{
    UmiTerminalControllerConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.initial_session_id = "terminal.primary";
    config.initial_profile_id = "";
    config.initial_title = "Terminal";
    config.working_directory = ".";
    config.history_capacity = 512U;
    config.event_capacity = 512U;
    return config;
}

UmiStatus umi_terminal_controller_create(
    const UmiTerminalControllerConfig *config,
    UmiTerminalController **out_controller)
{
    UmiTerminalController *controller;
    UmiStatus status;
    const char *active_id;
    if (config == NULL || out_controller == NULL || config->manager == NULL ||
        config->process_supervisor == NULL || config->task_queue == NULL ||
        config->clock == NULL || config->initial_session_id == NULL ||
        config->working_directory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_controller = NULL;
    controller = (UmiTerminalController *)calloc(1U, sizeof(*controller));
    if (controller == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    controller->manager = config->manager;
    controller->process_supervisor = config->process_supervisor;
    controller->task_queue = config->task_queue;
    controller->pipeline = config->diagnostic_pipeline;
    controller->operation_graph = config->operation_graph;
    controller->clock = config->clock;
    status = umi_terminal_profile_registry_create(&controller->profiles);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_profile_registry_add_platform_defaults(controller->profiles);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_history_create(config->history_capacity,
                                             &controller->history);
    }
    if (status == UMI_STATUS_OK) status = umi_terminal_tab_model_create(&controller->tabs);
    if (status == UMI_STATUS_OK) {
        active_id = config->initial_session_id;
        if (umi_terminal_manager_find(controller->manager, active_id) == NULL) {
            UmiTerminalSession *session = NULL;
            status = umi_terminal_manager_open(controller->manager, active_id,
                                               config->initial_title,
                                               config->working_directory,
                                               &session);
        }
    }
    if (status == UMI_STATUS_OK) {
        const char *profile_id = config->initial_profile_id != NULL &&
            config->initial_profile_id[0] != '\0'
            ? config->initial_profile_id
            : umi_terminal_profile_registry_default_id(controller->profiles);
        status = umi_terminal_tab_model_add(controller->tabs,
                                            config->initial_session_id,
                                            config->initial_title,
                                            profile_id != NULL ? profile_id : "default");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_split_model_create(config->initial_session_id,
                                                 &controller->splits);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_event_log_create(config->event_capacity,
                                               &controller->events);
    }
    if (status != UMI_STATUS_OK) {
        umi_terminal_controller_destroy(controller);
        return status;
    }
    emit_event(controller, UMI_TERMINAL_EVENT_SESSION_OPENED,
               config->initial_session_id, "Initial terminal session ready", 0);
    *out_controller = controller;
    return UMI_STATUS_OK;
}

void umi_terminal_controller_destroy(UmiTerminalController *controller)
{
    if (controller == NULL) return;
    umi_terminal_event_log_destroy(controller->events);
    umi_terminal_split_model_destroy(controller->splits);
    umi_terminal_tab_model_destroy(controller->tabs);
    umi_terminal_history_destroy(controller->history);
    umi_terminal_profile_registry_destroy(controller->profiles);
    free(controller);
}

UmiStatus umi_terminal_controller_open(UmiTerminalController *controller,
                                       const char *profile_id,
                                       const char *session_id,
                                       const char *title,
                                       const char *working_directory)
{
    UmiTerminalProfile profile;
    UmiTerminalSession *session = NULL;
    UmiStatus status;
    if (controller == NULL || session_id == NULL || title == NULL ||
        working_directory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (profile_id == NULL || profile_id[0] == '\0') {
        profile_id = umi_terminal_profile_registry_default_id(controller->profiles);
    }
    status = umi_terminal_profile_registry_find(controller->profiles,
                                                profile_id, &profile);
    if (status != UMI_STATUS_OK) return status;
    status = umi_terminal_manager_open(controller->manager, session_id, title,
                                       working_directory, &session);
    if (status == UMI_STATUS_OK) {
        size_t index;
        UmiTerminalEnvironment *environment = umi_terminal_session_environment(session);
        for (index = 0U; index < profile.environment_count &&
             status == UMI_STATUS_OK; ++index) {
            status = umi_terminal_environment_set(environment,
                                                  profile.environment[index].name,
                                                  profile.environment[index].value);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_tab_model_add(controller->tabs, session_id,
                                            title, profile_id);
    }
    if (status == UMI_STATUS_OK) status = umi_terminal_tab_model_activate(controller->tabs,
                                                                           session_id);
    if (status == UMI_STATUS_OK) emit_event(controller, UMI_TERMINAL_EVENT_SESSION_OPENED,
                                            session_id, title, 0);
    return status;
}

UmiStatus umi_terminal_controller_close(UmiTerminalController *controller,
                                        const char *session_id)
{
    UmiStatus status;
    uint64_t split_leaf_id = 0U;
    size_t index;
    if (controller == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_terminal_tab_model_count(controller->tabs) <= 1U) {
        return UMI_STATUS_BUSY;
    }
    for (index = 0U; index < umi_terminal_split_model_count(controller->splits);
         ++index) {
        UmiTerminalSplitSnapshot node;
        if (umi_terminal_split_model_at(controller->splits, index, &node) ==
                UMI_STATUS_OK && node.leaf &&
            strcmp(node.session_id, session_id) == 0) {
            split_leaf_id = node.node_id;
            break;
        }
    }
    status = umi_terminal_tab_model_remove(controller->tabs, session_id);
    if (status == UMI_STATUS_OK) status = umi_terminal_manager_close(controller->manager,
                                                                     session_id);
    if (status == UMI_STATUS_OK && split_leaf_id != 0U &&
        umi_terminal_split_model_count(controller->splits) > 1U) {
        status = umi_terminal_split_model_close(controller->splits,
                                                split_leaf_id);
        if (status == UMI_STATUS_OK) {
            emit_event(controller, UMI_TERMINAL_EVENT_LAYOUT_CHANGED,
                       session_id, "Terminal split closed", 0);
        }
    }
    if (status == UMI_STATUS_OK) emit_event(controller, UMI_TERMINAL_EVENT_SESSION_CLOSED,
                                            session_id, "Terminal session closed", 0);
    return status;
}

UmiStatus umi_terminal_controller_close_active(
    UmiTerminalController *controller)
{
    const char *session_id;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session_id = umi_terminal_tab_model_active_id(controller->tabs);
    return session_id != NULL
        ? umi_terminal_controller_close(controller, session_id)
        : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_terminal_controller_activate(UmiTerminalController *controller,
                                           const char *session_id)
{
    UmiStatus status;
    if (controller == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_tab_model_activate(controller->tabs, session_id);
    if (status == UMI_STATUS_OK) emit_event(controller,
        UMI_TERMINAL_EVENT_SESSION_ACTIVATED, session_id, "Terminal activated", 0);
    return status;
}

UmiStatus umi_terminal_controller_activate_relative(
    UmiTerminalController *controller,
    int direction)
{
    UmiStatus status;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_tab_model_activate_relative(controller->tabs, direction);
    if (status == UMI_STATUS_OK) emit_event(controller,
        UMI_TERMINAL_EVENT_SESSION_ACTIVATED,
        umi_terminal_tab_model_active_id(controller->tabs),
        "Terminal selection changed", 0);
    return status;
}

UmiTerminalSession *umi_terminal_controller_active_session(
    const UmiTerminalController *controller)
{
    const char *session_id = controller != NULL
        ? umi_terminal_tab_model_active_id(controller->tabs) : NULL;
    return session_id != NULL
        ? umi_terminal_manager_find(controller->manager, session_id) : NULL;
}

UmiStatus umi_terminal_controller_execute(UmiTerminalController *controller,
                                          const char *command,
                                          uint32_t timeout_ms,
                                          UmiCancellationToken *cancellation,
                                          int *out_exit_code)
{
    UmiTerminalSession *session;
    UmiTerminalSessionSnapshot snapshot;
    UmiTerminalTabSnapshot tab;
    UmiTerminalProfile profile;
    UmiTerminalCommand prepared;
    UmiTerminalHistoryEntry history = {0};
    UmiStatus status;
    int exit_code = -1;
    if (controller == NULL || command == NULL || command[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session = umi_terminal_controller_active_session(controller);
    if (session == NULL) return UMI_STATUS_NOT_FOUND;
    (void)umi_terminal_session_snapshot(session, &snapshot);
    status = umi_terminal_tab_model_find(controller->tabs,
                                         snapshot.session_id, &tab);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_profile_registry_find(
            controller->profiles, tab.profile_id, &profile);
    }
    if (status == UMI_STATUS_OK) {
        status = prepare_profile_command(&profile, command, &prepared);
    }
    if (status != UMI_STATUS_OK) return status;
    emit_event(controller, UMI_TERMINAL_EVENT_COMMAND_STARTED,
               snapshot.session_id, command, 0);
    publish_output(controller, "Terminal", UMI_OUTPUT_STREAM_SYSTEM, command);
    status = umi_terminal_session_execute_prepared(
        session, &prepared, command, timeout_ms, cancellation, &exit_code);
    (void)snprintf(history.session_id, sizeof(history.session_id), "%s",
                   snapshot.session_id);
    (void)snprintf(history.working_directory, sizeof(history.working_directory),
                   "%s", snapshot.working_directory);
    (void)snprintf(history.command, sizeof(history.command), "%s", command);
    history.timestamp_ns = now_ns(controller);
    history.exit_code = exit_code;
    history.completed = 1;
    (void)umi_terminal_history_append(controller->history, &history);
    emit_event(controller, UMI_TERMINAL_EVENT_COMMAND_FINISHED,
               snapshot.session_id, command, (int)status);
    {
        UmiTerminalTranscript *transcript = umi_terminal_session_transcript(session);
        size_t count = umi_terminal_transcript_count(transcript);
        if (count > 0U) {
            UmiTerminalTranscriptLine line;
            if (umi_terminal_transcript_at(transcript, count - 1U, &line) == UMI_STATUS_OK) {
                publish_output(controller, "Terminal",
                    line.stream == UMI_TERMINAL_STREAM_ERROR
                        ? UMI_OUTPUT_STREAM_ERROR : UMI_OUTPUT_STREAM_STANDARD,
                    line.text);
            }
        }
    }
    if (out_exit_code != NULL) *out_exit_code = exit_code;
    return status;
}

UmiStatus umi_terminal_controller_clear_active(UmiTerminalController *controller)
{
    UmiTerminalSession *session;
    UmiTerminalSessionSnapshot snapshot;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = umi_terminal_controller_active_session(controller);
    if (session == NULL) return UMI_STATUS_NOT_FOUND;
    (void)umi_terminal_session_snapshot(session, &snapshot);
    umi_terminal_transcript_clear(umi_terminal_session_transcript(session));
    emit_event(controller, UMI_TERMINAL_EVENT_TRANSCRIPT_CLEARED,
               snapshot.session_id, "Terminal transcript cleared", 0);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_controller_clear_history(
    UmiTerminalController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_terminal_history_clear(controller->history);
    emit_event(controller, UMI_TERMINAL_EVENT_HISTORY_CLEARED,
               "history", "Terminal history cleared", 0);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_controller_split_active(
    UmiTerminalController *controller,
    const char *new_session_id,
    const char *title,
    UmiTerminalOrientation orientation)
{
    size_t index;
    uint64_t active_leaf = 0U;
    const char *active_id;
    const char *profile_id;
    UmiTerminalTabSnapshot tab;
    UmiTerminalSessionSnapshot session;
    UmiStatus status;
    if (controller == NULL || new_session_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    active_id = umi_terminal_tab_model_active_id(controller->tabs);
    if (active_id == NULL ||
        umi_terminal_tab_model_find(controller->tabs, active_id, &tab) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    profile_id = tab.profile_id;
    (void)umi_terminal_session_snapshot(umi_terminal_controller_active_session(controller),
                                        &session);
    for (index = 0U; index < umi_terminal_split_model_count(controller->splits); ++index) {
        UmiTerminalSplitSnapshot node;
        if (umi_terminal_split_model_at(controller->splits, index, &node) == UMI_STATUS_OK &&
            node.leaf && strcmp(node.session_id, active_id) == 0) {
            active_leaf = node.node_id;
            break;
        }
    }
    if (active_leaf == 0U) return UMI_STATUS_NOT_FOUND;
    status = umi_terminal_controller_open(controller, profile_id, new_session_id,
                                          title, session.working_directory);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_split_model_split(controller->splits, active_leaf,
                                                new_session_id, orientation,
                                                0.5, NULL);
    }
    if (status == UMI_STATUS_OK) emit_event(controller,
        UMI_TERMINAL_EVENT_LAYOUT_CHANGED, new_session_id, "Terminal split created", 0);
    return status;
}

UmiStatus umi_terminal_controller_search_active(
    const UmiTerminalController *controller,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result)
{
    UmiTerminalSession *session;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = umi_terminal_controller_active_session(controller);
    return session != NULL
        ? umi_terminal_search_transcript(umi_terminal_session_transcript(session),
                                         query, out_result)
        : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_terminal_controller_snapshot(
    const UmiTerminalController *controller,
    UmiTerminalControllerSnapshot *out_snapshot)
{
    const char *active;
    const char *default_profile;
    UmiTerminalHistoryStats history;
    if (controller == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->profiles = umi_terminal_profile_registry_count(controller->profiles);
    out_snapshot->sessions = umi_terminal_manager_count(controller->manager);
    out_snapshot->tabs = umi_terminal_tab_model_count(controller->tabs);
    out_snapshot->split_nodes = umi_terminal_split_model_count(controller->splits);
    history = umi_terminal_history_stats(controller->history);
    out_snapshot->history_entries = history.count;
    out_snapshot->events = umi_terminal_event_log_count(controller->events);
    active = umi_terminal_tab_model_active_id(controller->tabs);
    default_profile = umi_terminal_profile_registry_default_id(controller->profiles);
    (void)snprintf(out_snapshot->active_session_id,
                   sizeof(out_snapshot->active_session_id), "%s",
                   active != NULL ? active : "");
    (void)snprintf(out_snapshot->default_profile_id,
                   sizeof(out_snapshot->default_profile_id), "%s",
                   default_profile != NULL ? default_profile : "");
    (void)umi_terminal_metrics_collect(controller->manager,
                                       controller->process_supervisor,
                                       controller->task_queue,
                                       &out_snapshot->metrics);
    if (controller->operation_graph != NULL) {
        (void)umi_build_graph_snapshot(controller->operation_graph,
                                       &out_snapshot->operation_graph);
    }
    out_snapshot->revision = controller->revision +
        umi_terminal_tab_model_revision(controller->tabs) +
        umi_terminal_split_model_revision(controller->splits) +
        umi_terminal_event_log_revision(controller->events) + history.revision;
    return UMI_STATUS_OK;
}

UmiTerminalProfileRegistry *umi_terminal_controller_profiles(UmiTerminalController *c)
{ return c != NULL ? c->profiles : NULL; }
UmiTerminalHistory *umi_terminal_controller_history(UmiTerminalController *c)
{ return c != NULL ? c->history : NULL; }
UmiTerminalTabModel *umi_terminal_controller_tabs(UmiTerminalController *c)
{ return c != NULL ? c->tabs : NULL; }
UmiTerminalSplitModel *umi_terminal_controller_splits(UmiTerminalController *c)
{ return c != NULL ? c->splits : NULL; }
UmiTerminalEventLog *umi_terminal_controller_events(UmiTerminalController *c)
{ return c != NULL ? c->events : NULL; }
UmiProcessSupervisor *umi_terminal_controller_process_supervisor(UmiTerminalController *c)
{ return c != NULL ? c->process_supervisor : NULL; }
UmiTaskQueue *umi_terminal_controller_task_queue(UmiTerminalController *c)
{ return c != NULL ? c->task_queue : NULL; }
UmiBuildGraph *umi_terminal_controller_operation_graph(UmiTerminalController *c)
{ return c != NULL ? c->operation_graph : NULL; }

UmiStatus umi_terminal_controller_retry_operation(
    UmiTerminalController *controller,
    const char *node_id)
{
    UmiStatus status;
    if (controller == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->operation_graph == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_build_graph_retry(controller->operation_graph, node_id);
    if (status == UMI_STATUS_OK) {
        emit_event(controller, UMI_TERMINAL_EVENT_TASK_CHANGED,
                   node_id, "Operation scheduled for retry", 0);
    }
    return status;
}

UmiStatus umi_terminal_controller_cancel_operation(
    UmiTerminalController *controller,
    const char *node_id)
{
    UmiStatus status;
    if (controller == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->operation_graph == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_build_graph_cancel(controller->operation_graph, node_id);
    if (status == UMI_STATUS_OK) {
        emit_event(controller, UMI_TERMINAL_EVENT_TASK_CHANGED,
                   node_id, "Operation cancelled", 0);
    }
    return status;
}
