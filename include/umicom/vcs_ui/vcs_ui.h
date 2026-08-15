/*-----------------------------------------------------------------------------
 * Framework-owned, toolkit-neutral Source Control Centre view factories.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_UI_VCS_UI_H
#define UMICOM_VCS_UI_VCS_UI_H
#include "umicom/ui/view_model.h"
#include "umicom/vcs/workspace.h"
#include "umicom/vcs/workspace_coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_VCS_UI_VIEW_SOURCE_CONTROL "umicom.vcs.source-control"
#define UMI_VCS_UI_VIEW_HISTORY "umicom.vcs.history"
#define UMI_VCS_UI_VIEW_BRANCHES "umicom.vcs.branches"
#define UMI_VCS_UI_VIEW_REMOTES "umicom.vcs.remotes"
#define UMI_VCS_UI_VIEW_DIFF "umicom.vcs.diff"
#define UMI_VCS_UI_VIEW_WORKSPACE_CHANGES "umicom.vcs.workspace.changes"
#define UMI_VCS_UI_VIEW_WORKSPACE_COMMIT "umicom.vcs.workspace.commit"
#define UMI_VCS_UI_VIEW_WORKSPACE_HISTORY "umicom.vcs.workspace.history"
#define UMI_VCS_UI_VIEW_WORKSPACE_BRANCHES "umicom.vcs.workspace.branches"
#define UMI_VCS_UI_VIEW_WORKSPACE_REMOTES "umicom.vcs.workspace.remotes"
#define UMI_VCS_UI_VIEW_WORKSPACE_CONFLICTS "umicom.vcs.workspace.conflicts"
#define UMI_VCS_UI_VIEW_WORKSPACE_DIFF "umicom.vcs.workspace.diff"
#define UMI_VCS_UI_VIEW_WORKSPACE_OPERATIONS "umicom.vcs.workspace.operations"
#define UMI_VCS_UI_VISIBLE_ROWS 20U
UmiStatus umi_vcs_ui_source_control_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_history_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_branches_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_remotes_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_diff_view_create(const char *view_id, const UmiVcsWorkspace *workspace, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_workspace_changes_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_commit_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_history_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_branches_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_remotes_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_conflicts_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_diff_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
UmiStatus umi_vcs_ui_workspace_operations_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view
);
#ifdef __cplusplus
}
#endif
#endif
