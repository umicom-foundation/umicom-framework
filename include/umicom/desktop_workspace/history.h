/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_workspace/history.h
 * PURPOSE: Inspect retained desktop checkpoints without replacing a live draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_WORKSPACE_HISTORY_H
#define UMICOM_DESKTOP_WORKSPACE_HISTORY_H
#include "umicom/desktop_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A copied row describes one retained revision. Only revision and status are
 * meaningful when status is not OK. Corrupt or missing records remain visible
 * instead of making older usable checkpoints disappear from the picker. */
typedef struct UmiDesktopWorkspaceHistoryRow {
    uint64_t revision;
    UmiStatus status;
    size_t noteCount;
    UmiDesktopWorkspaceTheme theme;
    unsigned fontPoints;
    char selectedTitle[UMI_DESKTOP_WORKSPACE_TITLE];
} UmiDesktopWorkspaceHistoryRow;
typedef struct UmiDesktopWorkspaceHistory {
    uint64_t currentRevision;
    size_t count;
    UmiDesktopWorkspaceHistoryRow rows[UMI_DESKTOP_WORKSPACE_HISTORY];
} UmiDesktopWorkspaceHistory;

/* Read newest first in one owned Data Server transaction. Missing/corrupt
 * checkpoint rows report their own status; backend, allocation, stale-head
 * and transaction failures leave the entire output unchanged. The live draft
 * is never replaced. This is inspection evidence, not authority to restore:
 * prepare a fresh restore review for the selected revision. Closed owners and
 * caller-owned transactions are refused. Use the workspace's owning thread
 * and keep output separate from its private storage. */
UmiStatus UmiDesktopWorkspaceReadHistory(UmiDesktopWorkspace *workspace,
    UmiDesktopWorkspaceHistory *outHistory);
#ifdef __cplusplus
}
#endif
#endif
