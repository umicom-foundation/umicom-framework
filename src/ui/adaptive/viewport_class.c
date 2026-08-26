/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/viewport_class.c
 *
 * PURPOSE:
 *   Bridge adaptive logical viewports to the canonical Design-System size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/viewport_class.h"

/* Reuse the canonical Design-System size classifier rather than duplicating thresholds. */
UmiStatus umi_adaptive_viewport_class_resolve(UmiAdaptiveViewport viewport,
                                              UmiDesignSizeClass *out_class)
{
    if (viewport.width <= 0 || viewport.height <= 0 || out_class == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_design_size_class_from_width(viewport.width, out_class);
}
