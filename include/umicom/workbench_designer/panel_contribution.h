/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/panel_contribution.h
 *
 * PURPOSE:
 *   Describe Framework-owned Layout Browser and Designer panels for composition
 *   by Desk, Studio and future applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PANEL_CONTRIBUTION_H
#define UMICOM_WORKBENCH_DESIGNER_PANEL_CONTRIBUTION_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer panel contribution data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerPanelContribution {
    char panel_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char title[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char icon_resource_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutDockRegion default_dock;
    uint32_t order;
    bool singleton;
    bool visible_by_default;
    bool requires_document;
    bool requires_selection;
} UmiWorkbenchDesignerPanelContribution;

/**
 * Represent the workbench designer panel catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPanelCatalogue {
    UmiWorkbenchDesignerPanelContribution panels[32U];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerPanelCatalogue;

/**
 * Initialise workbench designer panel catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_panel_catalogue_init(UmiWorkbenchDesignerPanelCatalogue *catalogue);
/**
 * Provide the workbench designer panel catalogue seed operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_panel_catalogue_seed(UmiWorkbenchDesignerPanelCatalogue *catalogue);
/**
 * Add workbench designer panel catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_designer_panel_catalogue_add(UmiWorkbenchDesignerPanelCatalogue *catalogue, const UmiWorkbenchDesignerPanelContribution *panel);
/**
 * Find workbench designer panel catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerPanelContribution *umi_workbench_designer_panel_catalogue_find(const UmiWorkbenchDesignerPanelCatalogue *catalogue, const char *panel_id);

#ifdef __cplusplus
}
#endif

#endif
