/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/media_adaptation.c
 *
 * PURPOSE:
 *   Resolve media-preview and transport placement for adaptive creative application layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/media_adaptation.h"

/* Prioritise media preview on compact screens and restore workstation regions as width grows. */
UmiDesignerMediaAdaptation umi_designer_media_adaptation_resolve(UmiDesignSizeClass size_class)
{
    UmiDesignerMediaAdaptation result;
    result.transport_overlay = size_class == UMI_DESIGN_SIZE_COMPACT;
    result.timeline_below = size_class >= UMI_DESIGN_SIZE_MEDIUM;
    result.inspector_overlay = size_class <= UMI_DESIGN_SIZE_MEDIUM;
    result.preview_fraction = size_class == UMI_DESIGN_SIZE_COMPACT ? 0.70 :
                              size_class == UMI_DESIGN_SIZE_MEDIUM ? 0.60 : 0.50;
    return result;
}
