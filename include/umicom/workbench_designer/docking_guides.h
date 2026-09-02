/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/docking_guides.h
 *
 * PURPOSE:
 *   Project drop targets into visual guide rectangles and retain the active
 *   docking decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DOCKING_GUIDES_H
#define UMICOM_WORKBENCH_DESIGNER_DOCKING_GUIDES_H

#include "umicom/workbench_designer/drop_target.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer docking guide data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDockingGuide {
    char guide_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerDropZone zone;
    UmiWorkbenchDesignerRect bounds;
    bool active;
    bool enabled;
} UmiWorkbenchDesignerDockingGuide;

/**
 * Represent the workbench designer docking guide model data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerDockingGuideModel {
    UmiWorkbenchDesignerDockingGuide guides[UMI_WORKBENCH_DESIGNER_MAX_GUIDES];
    size_t count;
    char active_guide_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerDockingGuideModel;

/**
 * Initialise workbench designer docking guides from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_docking_guides_init(UmiWorkbenchDesignerDockingGuideModel *model);
/**
 * Provide the workbench designer docking guides build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_docking_guides_build(UmiWorkbenchDesignerDockingGuideModel *model, const UmiWorkbenchDesignerDropTargetSet *targets, const char *target_node_id);
/**
 * Provide the workbench designer docking guides activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_docking_guides_activate(UmiWorkbenchDesignerDockingGuideModel *model, const char *guide_id);
/**
 * Provide the workbench designer docking guide active operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDockingGuide *umi_workbench_designer_docking_guide_active(const UmiWorkbenchDesignerDockingGuideModel *model);

#ifdef __cplusplus
}
#endif

#endif
