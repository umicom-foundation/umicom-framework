/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/types.h
 *
 * PURPOSE:
 *   Define stable visual-designer operation kinds, geometry, reusable layout
 *   operations, preview profiles and bounded authoring capacities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_TYPES_H
#define UMICOM_DESIGNER_TYPES_H

#include "umicom/declarative/declarative.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_MAX_HISTORY 128U
#define UMI_DESIGNER_MAX_SELECTION 64U
#define UMI_DESIGNER_MAX_PROJECT_DOCUMENTS 32U
#define UMI_DESIGNER_MAX_PALETTE_RESULTS 64U

typedef enum UmiDesignerOperationKind {
    UMI_DESIGNER_OP_ADD_COMPONENT = 1,
    UMI_DESIGNER_OP_REMOVE_COMPONENT = 2,
    UMI_DESIGNER_OP_SET_PROPERTY = 3,
    UMI_DESIGNER_OP_MOVE_COMPONENT = 4
} UmiDesignerOperationKind;

typedef enum UmiDesignerAlignment {
    UMI_DESIGNER_ALIGN_LEFT = 0,
    UMI_DESIGNER_ALIGN_HORIZONTAL_CENTRE,
    UMI_DESIGNER_ALIGN_RIGHT,
    UMI_DESIGNER_ALIGN_TOP,
    UMI_DESIGNER_ALIGN_VERTICAL_CENTRE,
    UMI_DESIGNER_ALIGN_BOTTOM
} UmiDesignerAlignment;

typedef enum UmiDesignerDistribution {
    UMI_DESIGNER_DISTRIBUTE_HORIZONTAL = 0,
    UMI_DESIGNER_DISTRIBUTE_VERTICAL
} UmiDesignerDistribution;

typedef enum UmiDesignerPreviewProfile {
    UMI_DESIGNER_PREVIEW_DESKTOP = 0,
    UMI_DESIGNER_PREVIEW_LAPTOP,
    UMI_DESIGNER_PREVIEW_TABLET_LANDSCAPE,
    UMI_DESIGNER_PREVIEW_TABLET_PORTRAIT,
    UMI_DESIGNER_PREVIEW_PHONE_LANDSCAPE,
    UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT,
    UMI_DESIGNER_PREVIEW_CUSTOM
} UmiDesignerPreviewProfile;

typedef struct UmiDesignerRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} UmiDesignerRect;

const char *umi_designer_operation_kind_text(UmiDesignerOperationKind kind);
const char *umi_designer_alignment_text(UmiDesignerAlignment alignment);
const char *umi_designer_distribution_text(UmiDesignerDistribution distribution);
const char *umi_designer_preview_profile_text(UmiDesignerPreviewProfile profile);

#ifdef __cplusplus
}
#endif

#endif
