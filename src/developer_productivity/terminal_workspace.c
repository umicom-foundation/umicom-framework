/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_workspace.c
 *
 * PURPOSE:
 *   Implement IDE-oriented terminal session ownership over the existing
 *   Framework terminal manager.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_workspace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperTerminalWorkspace {
    UmiTerminalManager *manager;
    char active_session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    uint64_t revision;
};

UmiStatus umi_developer_terminal_workspace_create(
    UmiClock *clock,
    UmiDeveloperTerminalWorkspace **out_workspace)
{
    UmiDeveloperTerminalWorkspace *workspace;
    UmiStatus status;

    if (clock == NULL || out_workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_workspace = NULL;
    workspace = (UmiDeveloperTerminalWorkspace *)calloc(
        1U, sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_terminal_manager_create(clock, &workspace->manager);
    if (status != UMI_STATUS_OK) {
        free(workspace);
        return status;
    }

    workspace->revision = 1U;
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

void umi_developer_terminal_workspace_destroy(
    UmiDeveloperTerminalWorkspace *workspace)
{
    if (workspace == NULL) return;

    umi_terminal_manager_destroy(workspace->manager);
    free(workspace);
}

UmiStatus umi_developer_terminal_workspace_open(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id,
    const char *title,
    const char *working_directory)
{
    UmiTerminalSession *session = NULL;
    UmiStatus status;

    if (workspace == NULL || session_id == NULL ||
        title == NULL || working_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_terminal_manager_count(workspace->manager) >=
        UMI_DEVELOPER_TERMINAL_SESSION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_terminal_manager_open(
        workspace->manager,
        session_id,
        title,
        working_directory,
        &session);

    if (status == UMI_STATUS_OK) {
        (void)snprintf(
            workspace->active_session_id,
            sizeof(workspace->active_session_id),
            "%s",
            session_id);
        workspace->revision += 1U;
    }

    return status;
}

UmiStatus umi_developer_terminal_workspace_close(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id)
{
    UmiStatus status;

    if (workspace == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_terminal_manager_close(workspace->manager, session_id);
    if (status == UMI_STATUS_OK) {
        if (strcmp(workspace->active_session_id, session_id) == 0) {
            workspace->active_session_id[0] = '\0';

            if (umi_terminal_manager_count(workspace->manager) > 0U) {
                UmiTerminalSession *session =
                    umi_terminal_manager_at(workspace->manager, 0U);
                UmiTerminalSessionSnapshot snapshot;

                if (session != NULL &&
                    umi_terminal_session_snapshot(
                        session, &snapshot) == UMI_STATUS_OK) {
                    (void)snprintf(
                        workspace->active_session_id,
                        sizeof(workspace->active_session_id),
                        "%s",
                        snapshot.session_id);
                }
            }
        }

        workspace->revision += 1U;
    }

    return status;
}

UmiStatus umi_developer_terminal_workspace_activate(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *session_id)
{
    if (workspace == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_terminal_manager_find(workspace->manager, session_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)snprintf(
        workspace->active_session_id,
        sizeof(workspace->active_session_id),
        "%s",
        session_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiTerminalSession *umi_developer_terminal_workspace_active(
    UmiDeveloperTerminalWorkspace *workspace)
{
    if (workspace == NULL || workspace->active_session_id[0] == '\0') {
        return NULL;
    }

    return umi_terminal_manager_find(
        workspace->manager,
        workspace->active_session_id);
}

UmiStatus umi_developer_terminal_workspace_execute(
    UmiDeveloperTerminalWorkspace *workspace,
    const char *command_text,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code)
{
    UmiTerminalSession *session;

    if (workspace == NULL || command_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    session = umi_developer_terminal_workspace_active(workspace);
    if (session == NULL) return UMI_STATUS_NOT_FOUND;

    return umi_terminal_session_execute(
        session,
        command_text,
        timeout_ms,
        cancellation,
        out_exit_code);
}

UmiStatus umi_developer_terminal_workspace_snapshot(
    const UmiDeveloperTerminalWorkspace *workspace,
    UmiDeveloperTerminalWorkspaceSnapshot *out_snapshot)
{
    if (workspace == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->session_count =
        umi_terminal_manager_count(workspace->manager);
    (void)snprintf(
        out_snapshot->active_session_id,
        sizeof(out_snapshot->active_session_id),
        "%s",
        workspace->active_session_id);
    out_snapshot->revision = workspace->revision;
    return UMI_STATUS_OK;
}
