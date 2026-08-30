/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/sidebar_adaptation.c
 *
 * PURPOSE:
 *   Adapt sidebar presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/sidebar_adaptation.h"

/* Collapse sidebars on medium views and convert them to overlays on compact views. */
UmiAdaptiveSidebarDecision umi_adaptive_sidebar_adaptation_resolve(UmiDesignSizeClass size_class, int32_t preferred_extent)
{
    UmiAdaptiveSidebarDecision decision;
    decision.extent = preferred_extent > 0 ? preferred_extent : 280;
    decision.presentation = size_class == UMI_DESIGN_SIZE_COMPACT ? UMI_ADAPTIVE_PRESENTATION_OVERLAY :
                            size_class == UMI_DESIGN_SIZE_MEDIUM ? UMI_ADAPTIVE_PRESENTATION_COLLAPSED :
                            UMI_ADAPTIVE_PRESENTATION_INLINE;
    return decision;
}
