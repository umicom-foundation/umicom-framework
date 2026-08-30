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


void umi_workbench_designer_workspace_init(UmiWorkbenchDesignerWorkspace *workspace)
{
    if (workspace == NULL) return;
    (void)memset(workspace, 0, sizeof(*workspace));
    workspace->mode = UMI_WORKBENCH_DESIGNER_MODE_BROWSE;
}

UmiStatus umi_workbench_designer_workspace_build(
    UmiWorkbenchDesignerWorkspace *workspace,
    const UmiWorkbenchDesignerPanelCatalogue *catalogue,
    UmiWorkbenchDesignerMode mode,
    bool document_open,
    bool selection_available)
{
    size_t index;
    if (workspace == NULL || catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_workspace_init(workspace);
    workspace->mode = mode;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiWorkbenchDesignerPanelContribution *panel = &catalogue->panels[index];
        UmiWorkbenchDesignerWorkspacePanelState *state = &workspace->panels[workspace->count++];
        (void)memset(state, 0, sizeof(*state));
        (void)umi_workbench_designer_copy_text(state->panel_id, sizeof(state->panel_id), panel->panel_id);
        state->available = (!panel->requires_document || document_open) &&
            (!panel->requires_selection || selection_available);
        state->visible = panel->visible_by_default && state->available;
    }
    if (workspace->count > 0U) {
        const char *preferred = mode == UMI_WORKBENCH_DESIGNER_MODE_BROWSE
            ? "umicom.layout.browser" : "umicom.layout.canvas";
        (void)umi_workbench_designer_workspace_activate(workspace, preferred);
    }
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_workspace_activate(
    UmiWorkbenchDesignerWorkspace *workspace,
    const char *panel_id)
{
    size_t index;
    bool found = false;
    if (workspace == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < workspace->count; ++index) {
        bool active = strcmp(workspace->panels[index].panel_id, panel_id) == 0 &&
            workspace->panels[index].available;
        workspace->panels[index].active = active;
        if (active) {
            workspace->panels[index].visible = true;
            found = true;
        }
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        workspace->active_panel_id, sizeof(workspace->active_panel_id), panel_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_workspace_set_visible(
    UmiWorkbenchDesignerWorkspace *workspace,
    const char *panel_id,
    bool visible)
{
    size_t index;
    if (workspace == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < workspace->count; ++index) {
        if (strcmp(workspace->panels[index].panel_id, panel_id) == 0) {
            if (visible && !workspace->panels[index].available) return UMI_STATUS_INVALID_STATE;
            workspace->panels[index].visible = visible;
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
