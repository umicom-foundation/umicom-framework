/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_workspace.h
 *
 * PURPOSE:
 *   Compose the existing Framework terminal manager/session platform into an
 *   IDE-oriented workspace with active-session state and safe command execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_WORKSPACE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_WORKSPACE_H

#include "umicom/developer_productivity/types.h"
#include "umicom/terminal/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY 32U

/**
 * Represent the developer terminal workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperTerminalWorkspace
    UmiDeveloperTerminalWorkspace;

/**
 * Represent the developer terminal workspace snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperTerminalWorkspaceSnapshot {
    size_t session_count;
    char active_session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperTerminalWorkspaceSnapshot;

/**
 * Initialise developer terminal workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_terminal_workspace_create(
    UmiClock *clock,
    UmiDeveloperTerminalWorkspace **out_workspace);

/**
 * Release or reset state held by developer terminal workspace so the same storage can be
 * reused safely.
 */
void umi_developer_terminal_workspace_destroy(
    UmiDeveloperTerminalWorkspace *workspace);

/**
 * Provide the developer terminal workspace open operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_workspace_open(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id,
    const char *title,
    const char *working_directory);

/**
 * Provide the developer terminal workspace close operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_workspace_close(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id);

/**
 * Provide the developer terminal workspace activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_workspace_activate(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id);

/**
 * Provide the developer terminal workspace active operation used by this module and its
 * client applications.
 */
UmiTerminalSession *umi_developer_terminal_workspace_active(
    UmiDeveloperTerminalWorkspace *workspace);

/**
 * Perform developer terminal workspace through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_developer_terminal_workspace_execute(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *command_text,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code);

/**
 * Provide the developer terminal workspace snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_terminal_workspace_snapshot(
    const UmiDeveloperTerminalWorkspace *workspace,
    UmiDeveloperTerminalWorkspaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
