/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_ui_workspace_views.c
 *
 * PURPOSE:
 *   Verify every professional source-control pane is projected from one
 *   Framework coordinator and exposes capability-aware actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ui/view_model.h"
#include "umicom/vcs_ui/vcs_ui.h"
#include "vcs_fixture.h"

typedef UmiStatus (*ViewFactory)(
    const char *, UmiVcsWorkspaceCoordinator *, UmiUiViewModel **);

static void verify_view(ViewFactory factory, const char *view_id,
                        const char *expected_kind,
                        UmiVcsWorkspaceCoordinator *coordinator)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(factory(view_id, coordinator, &view) == UMI_STATUS_OK);
    assert(view != NULL);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiTestVcsFixture fixture = {0};
    UmiVcsProvider provider = test_vcs_provider(&fixture);
    UmiVcsWorkspace *workspace = NULL;
    UmiVcsWorkspaceCoordinator *coordinator = NULL;
    UmiUiViewModel *changes = NULL;
    UmiUiCommandViewAction action;

    assert(umi_vcs_workspace_create(".", &provider, 1, &workspace) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_refresh(workspace, 20U) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_create(workspace, &coordinator) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_set_commit_message(
               coordinator, "feat: professional source control") ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_open_selected_diff(
               coordinator, 1) == UMI_STATUS_OK);

    verify_view(umi_vcs_ui_workspace_changes_view_create, "changes",
                "source-control-changes", coordinator);
    verify_view(umi_vcs_ui_workspace_commit_view_create, "commit",
                "source-control-commit", coordinator);
    verify_view(umi_vcs_ui_workspace_history_view_create, "history",
                "source-control-history", coordinator);
    verify_view(umi_vcs_ui_workspace_branches_view_create, "branches",
                "source-control-branches", coordinator);
    verify_view(umi_vcs_ui_workspace_remotes_view_create, "remotes",
                "source-control-remotes", coordinator);
    verify_view(umi_vcs_ui_workspace_conflicts_view_create, "conflicts",
                "source-control-conflicts", coordinator);
    verify_view(umi_vcs_ui_workspace_diff_view_create, "diff",
                "source-control-diff", coordinator);
    verify_view(umi_vcs_ui_workspace_operations_view_create, "operations",
                "source-control-operations", coordinator);

    assert(umi_vcs_ui_workspace_changes_view_create(
               "changes.actions", coordinator, &changes) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(changes, 0U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.vcs.refresh") == 0);
    assert(umi_ui_command_view_action_at(changes, 4U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.vcs.unstage-selected") == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(changes);

    umi_vcs_workspace_coordinator_destroy(coordinator);
    umi_vcs_workspace_destroy(workspace);
    return 0;
}
