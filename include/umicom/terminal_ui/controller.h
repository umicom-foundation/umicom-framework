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

/**
 * Represent the terminal controller config data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the terminal controller data shared with callers of this public contract.
 */
typedef struct UmiTerminalController UmiTerminalController;

/**
 * Provide the terminal controller config default operation used by this module and its
 * client applications.
 */
UmiTerminalControllerConfig umi_terminal_controller_config_default(void);
/**
 * Initialise terminal controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_controller_create(
    const UmiTerminalControllerConfig *config,
    UmiTerminalController **out_controller);
/**
 * Release or reset state held by terminal controller so the same storage can be reused
 * safely.
 */
void umi_terminal_controller_destroy(UmiTerminalController *controller);
/**
 * Provide the terminal controller open operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_controller_open(UmiTerminalController *controller,
                                       const char *profile_id,
                                       const char *session_id,
                                       const char *title,
                                       const char *working_directory);
/**
 * Provide the terminal controller close operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_controller_close(UmiTerminalController *controller,
                                        const char *session_id);
/**
 * Provide the terminal controller close active operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_close_active(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_controller_activate(UmiTerminalController *controller,
                                           const char *session_id);
/**
 * Provide the terminal controller activate relative operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_activate_relative(
    UmiTerminalController *controller,
    int direction);
/**
 * Perform terminal controller through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_terminal_controller_execute(UmiTerminalController *controller,
                                          const char *command,
                                          uint32_t timeout_ms,
                                          UmiCancellationToken *cancellation,
                                          int *out_exit_code);
/**
 * Provide the terminal controller clear active operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_clear_active(UmiTerminalController *controller);
/**
 * Provide the terminal controller clear history operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_clear_history(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller split active operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_split_active(
    UmiTerminalController *controller,
    const char *new_session_id,
    const char *title,
    UmiTerminalOrientation orientation);
/**
 * Provide the terminal controller search active operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_search_active(
    const UmiTerminalController *controller,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result);
/**
 * Provide the terminal controller snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_controller_snapshot(
    const UmiTerminalController *controller,
    UmiTerminalControllerSnapshot *out_snapshot);
/**
 * Provide the terminal controller active session operation used by this module and its
 * client applications.
 */
UmiTerminalSession *umi_terminal_controller_active_session(
    const UmiTerminalController *controller);
/**
 * Provide the terminal controller profiles operation used by this module and its client
 * applications.
 */
UmiTerminalProfileRegistry *umi_terminal_controller_profiles(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller history operation used by this module and its client
 * applications.
 */
UmiTerminalHistory *umi_terminal_controller_history(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller tabs operation used by this module and its client
 * applications.
 */
UmiTerminalTabModel *umi_terminal_controller_tabs(UmiTerminalController *controller);
/**
 * Provide the terminal controller splits operation used by this module and its client
 * applications.
 */
UmiTerminalSplitModel *umi_terminal_controller_splits(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller events operation used by this module and its client
 * applications.
 */
UmiTerminalEventLog *umi_terminal_controller_events(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller process supervisor operation used by this module and its
 * client applications.
 */
UmiProcessSupervisor *umi_terminal_controller_process_supervisor(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller task queue operation used by this module and its client
 * applications.
 */
UmiTaskQueue *umi_terminal_controller_task_queue(UmiTerminalController *controller);
/**
 * Provide the terminal controller operation graph operation used by this module and its
 * client applications.
 */
UmiBuildGraph *umi_terminal_controller_operation_graph(
    UmiTerminalController *controller);
/**
 * Provide the terminal controller retry operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_retry_operation(
    UmiTerminalController *controller,
    const char *node_id);
/**
 * Provide the terminal controller cancel operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_controller_cancel_operation(
    UmiTerminalController *controller,
    const char *node_id);

#ifdef __cplusplus
}
#endif
#endif
