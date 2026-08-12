/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/types.c
 *
 * PURPOSE:
 *   Implement designer enum helpers shared by history, inspection, layout,
 *   preview and application reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below keep names for reusable designer operations in one place
 * so Studio and future applications do not duplicate enum-to-text mappings.
 */

#include "umicom/designer/types.h"

const char *umi_designer_operation_kind_text(UmiDesignerOperationKind kind)
{
    switch (kind) {
        case UMI_DESIGNER_OP_ADD_COMPONENT: return "add-component";
        case UMI_DESIGNER_OP_REMOVE_COMPONENT: return "remove-component";
        case UMI_DESIGNER_OP_SET_PROPERTY: return "set-property";
        case UMI_DESIGNER_OP_MOVE_COMPONENT: return "move-component";
        default: return "unknown";
    }
}

const char *umi_designer_alignment_text(UmiDesignerAlignment alignment)
{
    switch (alignment) {
        case UMI_DESIGNER_ALIGN_LEFT: return "left";
        case UMI_DESIGNER_ALIGN_HORIZONTAL_CENTRE: return "horizontal-centre";
        case UMI_DESIGNER_ALIGN_RIGHT: return "right";
        case UMI_DESIGNER_ALIGN_TOP: return "top";
        case UMI_DESIGNER_ALIGN_VERTICAL_CENTRE: return "vertical-centre";
        case UMI_DESIGNER_ALIGN_BOTTOM: return "bottom";
        default: return "unknown";
    }
}

const char *umi_designer_distribution_text(UmiDesignerDistribution distribution)
{
    switch (distribution) {
        case UMI_DESIGNER_DISTRIBUTE_HORIZONTAL: return "horizontal";
        case UMI_DESIGNER_DISTRIBUTE_VERTICAL: return "vertical";
        default: return "unknown";
    }
}

const char *umi_designer_preview_profile_text(UmiDesignerPreviewProfile profile)
{
    switch (profile) {
        case UMI_DESIGNER_PREVIEW_DESKTOP: return "Desktop";
        case UMI_DESIGNER_PREVIEW_LAPTOP: return "Laptop";
        case UMI_DESIGNER_PREVIEW_TABLET_LANDSCAPE: return "Tablet Landscape";
        case UMI_DESIGNER_PREVIEW_TABLET_PORTRAIT: return "Tablet Portrait";
        case UMI_DESIGNER_PREVIEW_PHONE_LANDSCAPE: return "Phone Landscape";
        case UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT: return "Phone Portrait";
        case UMI_DESIGNER_PREVIEW_CUSTOM: return "Custom";
        default: return "Unknown";
    }
}
