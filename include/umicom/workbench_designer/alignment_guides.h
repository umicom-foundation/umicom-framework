/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/alignment_guides.h
 *
 * PURPOSE:
 *   Calculate dynamic left, centre, right, top, middle and bottom alignment
 *   guides against nearby canvas items during move and resize operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_GUIDES_H
#define UMICOM_WORKBENCH_DESIGNER_ALIGNMENT_GUIDES_H

#include "umicom/workbench_designer/canvas.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerAlignmentGuideKind {
    UMI_WORKBENCH_DESIGNER_GUIDE_LEFT = 1,
    UMI_WORKBENCH_DESIGNER_GUIDE_HORIZONTAL_CENTRE = 2,
    UMI_WORKBENCH_DESIGNER_GUIDE_RIGHT = 3,
    UMI_WORKBENCH_DESIGNER_GUIDE_TOP = 4,
    UMI_WORKBENCH_DESIGNER_GUIDE_VERTICAL_CENTRE = 5,
    UMI_WORKBENCH_DESIGNER_GUIDE_BOTTOM = 6
} UmiWorkbenchDesignerAlignmentGuideKind;

typedef struct UmiWorkbenchDesignerAlignmentGuide {
    UmiWorkbenchDesignerAlignmentGuideKind kind;
    char source_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    double position;
    double start;
    double end;
    double distance;
    bool vertical;
} UmiWorkbenchDesignerAlignmentGuide;

typedef struct UmiWorkbenchDesignerAlignmentGuideModel {
    UmiWorkbenchDesignerAlignmentGuide guides[UMI_WORKBENCH_DESIGNER_MAX_GUIDES];
    size_t count;
    double threshold;
    uint64_t canvas_revision;
    uint64_t revision;
} UmiWorkbenchDesignerAlignmentGuideModel;

void umi_workbench_designer_alignment_guides_init(UmiWorkbenchDesignerAlignmentGuideModel *model, double threshold);
UmiStatus umi_workbench_designer_alignment_guides_build(UmiWorkbenchDesignerAlignmentGuideModel *model, const UmiWorkbenchDesignerCanvas *canvas, const char *moving_node_id, UmiWorkbenchDesignerRect moving_bounds);
const UmiWorkbenchDesignerAlignmentGuide *umi_workbench_designer_alignment_guide_at(const UmiWorkbenchDesignerAlignmentGuideModel *model, size_t index);

#ifdef __cplusplus
}
#endif

#endif
