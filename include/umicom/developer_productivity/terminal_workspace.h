/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_workspace.h
 *
 * PURPOSE:
 *   Compose the existing Framework terminal manager/session platform into an
 *   IDE-oriented workspace with active-session state and safe command execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_WORKSPACE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_WORKSPACE_H

#include "umicom/developer_productivity/types.h"
#include "umicom/terminal/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY 32U

typedef struct UmiDeveloperTerminalWorkspace
    UmiDeveloperTerminalWorkspace;

typedef struct UmiDeveloperTerminalWorkspaceSnapshot {
    size_t session_count;
    char active_session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperTerminalWorkspaceSnapshot;

UmiStatus umi_developer_terminal_workspace_create(
    UmiClock *clock,
    UmiDeveloperTerminalWorkspace **out_workspace);

void umi_developer_terminal_workspace_destroy(
    UmiDeveloperTerminalWorkspace *workspace);

UmiStatus umi_developer_terminal_workspace_open(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id,
    const char *title,
    const char *working_directory);

UmiStatus umi_developer_terminal_workspace_close(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id);

UmiStatus umi_developer_terminal_workspace_activate(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id);

UmiTerminalSession *umi_developer_terminal_workspace_active(
    UmiDeveloperTerminalWorkspace *workspace);

UmiStatus umi_developer_terminal_workspace_execute(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *command_text,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code);

UmiStatus umi_developer_terminal_workspace_snapshot(
    const UmiDeveloperTerminalWorkspace *workspace,
    UmiDeveloperTerminalWorkspaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
