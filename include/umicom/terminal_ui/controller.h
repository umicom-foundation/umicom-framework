/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/controller.h
 *
 * PURPOSE:
 *   Coordinate reusable terminal profiles, sessions, tabs, splits, history,
 *   output, process supervision and task monitoring for application frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_CONTROLLER_H
#define UMICOM_TERMINAL_UI_CONTROLLER_H

#include "umicom/diagnostics/pipeline.h"
#include "umicom/terminal_ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalControllerConfig {
    UmiTerminalManager *manager;
    UmiProcessSupervisor *process_supervisor;
    UmiTaskQueue *task_queue;
    UmiDiagnosticPipeline *diagnostic_pipeline;
    UmiBuildGraph *operation_graph;
    UmiClock *clock;
    const char *initial_session_id;
    const char *initial_profile_id;
    const char *initial_title;
    const char *working_directory;
    size_t history_capacity;
    size_t event_capacity;
} UmiTerminalControllerConfig;

typedef struct UmiTerminalController UmiTerminalController;

UmiTerminalControllerConfig umi_terminal_controller_config_default(void);
UmiStatus umi_terminal_controller_create(
    const UmiTerminalControllerConfig *config,
    UmiTerminalController **out_controller);
void umi_terminal_controller_destroy(UmiTerminalController *controller);
UmiStatus umi_terminal_controller_open(UmiTerminalController *controller,
                                       const char *profile_id,
                                       const char *session_id,
                                       const char *title,
                                       const char *working_directory);
UmiStatus umi_terminal_controller_close(UmiTerminalController *controller,
                                        const char *session_id);
UmiStatus umi_terminal_controller_close_active(
    UmiTerminalController *controller);
UmiStatus umi_terminal_controller_activate(UmiTerminalController *controller,
                                           const char *session_id);
UmiStatus umi_terminal_controller_activate_relative(
    UmiTerminalController *controller,
    int direction);
UmiStatus umi_terminal_controller_execute(UmiTerminalController *controller,
                                          const char *command,
                                          uint32_t timeout_ms,
                                          UmiCancellationToken *cancellation,
                                          int *out_exit_code);
UmiStatus umi_terminal_controller_clear_active(UmiTerminalController *controller);
UmiStatus umi_terminal_controller_clear_history(
    UmiTerminalController *controller);
UmiStatus umi_terminal_controller_split_active(
    UmiTerminalController *controller,
    const char *new_session_id,
    const char *title,
    UmiTerminalOrientation orientation);
UmiStatus umi_terminal_controller_search_active(
    const UmiTerminalController *controller,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result);
UmiStatus umi_terminal_controller_snapshot(
    const UmiTerminalController *controller,
    UmiTerminalControllerSnapshot *out_snapshot);
UmiTerminalSession *umi_terminal_controller_active_session(
    const UmiTerminalController *controller);
UmiTerminalProfileRegistry *umi_terminal_controller_profiles(
    UmiTerminalController *controller);
UmiTerminalHistory *umi_terminal_controller_history(
    UmiTerminalController *controller);
UmiTerminalTabModel *umi_terminal_controller_tabs(UmiTerminalController *controller);
UmiTerminalSplitModel *umi_terminal_controller_splits(
    UmiTerminalController *controller);
UmiTerminalEventLog *umi_terminal_controller_events(
    UmiTerminalController *controller);
UmiProcessSupervisor *umi_terminal_controller_process_supervisor(
    UmiTerminalController *controller);
UmiTaskQueue *umi_terminal_controller_task_queue(UmiTerminalController *controller);
UmiBuildGraph *umi_terminal_controller_operation_graph(
    UmiTerminalController *controller);
UmiStatus umi_terminal_controller_retry_operation(
    UmiTerminalController *controller,
    const char *node_id);
UmiStatus umi_terminal_controller_cancel_operation(
    UmiTerminalController *controller,
    const char *node_id);

#ifdef __cplusplus
}
#endif
#endif
