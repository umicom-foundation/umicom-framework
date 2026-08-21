/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/workspace.h
 *
 * PURPOSE:
 *   Compose panel visibility and active focus for the Layout Browser and
 *   Designer workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_WORKSPACE_H
#define UMICOM_WORKBENCH_DESIGNER_WORKSPACE_H

#include "umicom/workbench_designer/panel_contribution.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerWorkspacePanelState {
    char panel_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool visible;
    bool active;
    bool available;
} UmiWorkbenchDesignerWorkspacePanelState;

typedef struct UmiWorkbenchDesignerWorkspace {
    UmiWorkbenchDesignerWorkspacePanelState panels[32U];
    size_t count;
    char active_panel_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerMode mode;
    uint64_t revision;
} UmiWorkbenchDesignerWorkspace;

void umi_workbench_designer_workspace_init(UmiWorkbenchDesignerWorkspace *workspace);
UmiStatus umi_workbench_designer_workspace_build(UmiWorkbenchDesignerWorkspace *workspace, const UmiWorkbenchDesignerPanelCatalogue *catalogue, UmiWorkbenchDesignerMode mode, bool document_open, bool selection_available);
UmiStatus umi_workbench_designer_workspace_activate(UmiWorkbenchDesignerWorkspace *workspace, const char *panel_id);
UmiStatus umi_workbench_designer_workspace_set_visible(UmiWorkbenchDesignerWorkspace *workspace, const char *panel_id, bool visible);

#ifdef __cplusplus
}
#endif

#endif
