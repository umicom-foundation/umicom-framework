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

typedef struct UmiWorkbenchDesignerPanelCatalogue {
    UmiWorkbenchDesignerPanelContribution panels[32U];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerPanelCatalogue;

void umi_workbench_designer_panel_catalogue_init(UmiWorkbenchDesignerPanelCatalogue *catalogue);
UmiStatus umi_workbench_designer_panel_catalogue_seed(UmiWorkbenchDesignerPanelCatalogue *catalogue);
UmiStatus umi_workbench_designer_panel_catalogue_add(UmiWorkbenchDesignerPanelCatalogue *catalogue, const UmiWorkbenchDesignerPanelContribution *panel);
const UmiWorkbenchDesignerPanelContribution *umi_workbench_designer_panel_catalogue_find(const UmiWorkbenchDesignerPanelCatalogue *catalogue, const char *panel_id);

#ifdef __cplusplus
}
#endif

#endif
