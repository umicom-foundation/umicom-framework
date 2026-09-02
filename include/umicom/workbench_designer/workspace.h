/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/workspace.h
 *
 * PURPOSE:
 *   Compose panel visibility and active focus for the Layout Browser and
 *   Designer workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_WORKSPACE_H
#define UMICOM_WORKBENCH_DESIGNER_WORKSPACE_H

#include "umicom/workbench_designer/panel_contribution.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer workspace panel state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerWorkspacePanelState {
    char panel_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool visible;
    bool active;
    bool available;
} UmiWorkbenchDesignerWorkspacePanelState;

/**
 * Represent the workbench designer workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerWorkspace {
    UmiWorkbenchDesignerWorkspacePanelState panels[32U];
    size_t count;
    char active_panel_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerMode mode;
    uint64_t revision;
} UmiWorkbenchDesignerWorkspace;

/**
 * Initialise workbench designer workspace from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_workspace_init(UmiWorkbenchDesignerWorkspace *workspace);
/**
 * Provide the workbench designer workspace build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_workspace_build(UmiWorkbenchDesignerWorkspace *workspace, const UmiWorkbenchDesignerPanelCatalogue *catalogue, UmiWorkbenchDesignerMode mode, bool document_open, bool selection_available);
/**
 * Provide the workbench designer workspace activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_workspace_activate(UmiWorkbenchDesignerWorkspace *workspace, const char *panel_id);
/**
 * Provide the workbench designer workspace set visible operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_workspace_set_visible(UmiWorkbenchDesignerWorkspace *workspace, const char *panel_id, bool visible);

#ifdef __cplusplus
}
#endif

#endif
