/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_ui_views.c
 *
 * PURPOSE:
 *   Implement the test vcs ui views behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Toolkit-neutral Source Control Centre view tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include "vcs_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestVcsFixture fixture = {0}; UmiVcsProvider provider = test_vcs_provider(&fixture); UmiVcsWorkspace *workspace = NULL; UmiUiViewModel *view = NULL; UmiUiValue value;
    assert(umi_vcs_workspace_create(".", &provider, 1, &workspace) == UMI_STATUS_OK); assert(umi_vcs_workspace_refresh(workspace, 20U) == UMI_STATUS_OK);
    assert(umi_vcs_workspace_open_diff(workspace, "src/main.c", 0) == UMI_STATUS_OK);
    assert(umi_vcs_ui_source_control_view_create("source", workspace, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "umicom.view-kind", &value) == UMI_STATUS_OK); assert(value.kind == UMI_UI_VALUE_STRING); umi_ui_view_model_destroy(view);
    assert(umi_vcs_ui_history_view_create("history", workspace, &view) == UMI_STATUS_OK); umi_ui_view_model_destroy(view);
    assert(umi_vcs_ui_branches_view_create("branches", workspace, &view) == UMI_STATUS_OK); umi_ui_view_model_destroy(view);
    assert(umi_vcs_ui_remotes_view_create("remotes", workspace, &view) == UMI_STATUS_OK); umi_ui_view_model_destroy(view);
    assert(umi_vcs_ui_diff_view_create("diff", workspace, &view) == UMI_STATUS_OK); umi_ui_view_model_destroy(view);
    umi_vcs_workspace_destroy(workspace); return 0;
}
