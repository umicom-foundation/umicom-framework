/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/workspace.c
 *
 * PURPOSE:
 *   Implement workbench panel availability and focus without creating frontend
 *   widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/workspace.h"
#include "internal.h"


/*
 * Initialise workbench designer workspace from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_workspace_init(UmiWorkbenchDesignerWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return;
    (void)memset(workspace, 0, sizeof(*workspace));
    workspace->mode = UMI_WORKBENCH_DESIGNER_MODE_BROWSE;
}

/*
 * Provide the workbench designer workspace build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_workspace_build(
    UmiWorkbenchDesignerWorkspace *workspace,
    const UmiWorkbenchDesignerPanelCatalogue *catalogue,
    UmiWorkbenchDesignerMode mode,
    bool document_open,
    bool selection_available)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_workspace_init(workspace);
    workspace->mode = mode;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiWorkbenchDesignerPanelContribution *panel = &catalogue->panels[index];
        UmiWorkbenchDesignerWorkspacePanelState *state = &workspace->panels[workspace->count++];
        (void)memset(state, 0, sizeof(*state));
        (void)umi_workbench_designer_copy_text(state->panel_id, sizeof(state->panel_id), panel->panel_id);
        state->available = (!panel->requires_document || document_open) &&
            (!panel->requires_selection || selection_available);
        state->visible = panel->visible_by_default && state->available;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workspace->count > 0U) {
        const char *preferred = mode == UMI_WORKBENCH_DESIGNER_MODE_BROWSE
            ? "umicom.layout.browser" : "umicom.layout.canvas";
        (void)umi_workbench_designer_workspace_activate(workspace, preferred);
    }
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer workspace activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_workspace_activate(
    UmiWorkbenchDesignerWorkspace *workspace,
    const char *panel_id)
{
    size_t index;
    bool found = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->count; ++index) {
        bool active = strcmp(workspace->panels[index].panel_id, panel_id) == 0 &&
            workspace->panels[index].available;
        workspace->panels[index].active = active;
        /* Apply this operation only while the related capability or state is available. */
        if (active) {
            workspace->panels[index].visible = true;
            found = true;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!found) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        workspace->active_panel_id, sizeof(workspace->active_panel_id), panel_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer workspace set visible operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_workspace_set_visible(
    UmiWorkbenchDesignerWorkspace *workspace,
    const char *panel_id,
    bool visible)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(workspace->panels[index].panel_id, panel_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (visible && !workspace->panels[index].available) return UMI_STATUS_INVALID_STATE;
            workspace->panels[index].visible = visible;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!visible && workspace->panels[index].active) {
                workspace->panels[index].active = false;
                workspace->active_panel_id[0] = '\0';
            }
            workspace->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
