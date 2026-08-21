/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/docking_guides.h
 *
 * PURPOSE:
 *   Project drop targets into visual guide rectangles and retain the active
 *   docking decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DOCKING_GUIDES_H
#define UMICOM_WORKBENCH_DESIGNER_DOCKING_GUIDES_H

#include "umicom/workbench_designer/drop_target.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerDockingGuide {
    char guide_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerDropZone zone;
    UmiWorkbenchDesignerRect bounds;
    bool active;
    bool enabled;
} UmiWorkbenchDesignerDockingGuide;

typedef struct UmiWorkbenchDesignerDockingGuideModel {
    UmiWorkbenchDesignerDockingGuide guides[UMI_WORKBENCH_DESIGNER_MAX_GUIDES];
    size_t count;
    char active_guide_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerDockingGuideModel;

void umi_workbench_designer_docking_guides_init(UmiWorkbenchDesignerDockingGuideModel *model);
UmiStatus umi_workbench_designer_docking_guides_build(UmiWorkbenchDesignerDockingGuideModel *model, const UmiWorkbenchDesignerDropTargetSet *targets, const char *target_node_id);
UmiStatus umi_workbench_designer_docking_guides_activate(UmiWorkbenchDesignerDockingGuideModel *model, const char *guide_id);
const UmiWorkbenchDesignerDockingGuide *umi_workbench_designer_docking_guide_active(const UmiWorkbenchDesignerDockingGuideModel *model);

#ifdef __cplusplus
}
#endif

#endif
