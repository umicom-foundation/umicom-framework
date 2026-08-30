/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/panel_adaptation.c
 *
 * PURPOSE:
 *   Adapt panel presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/panel_adaptation.h"

/* Keep central content inline and convert auxiliary compact panels to overlays. */
UmiAdaptivePanelDecision umi_adaptive_panel_adaptation_resolve(UmiDesignSizeClass size_class, UmiUiPlacement preferred)
{
    UmiAdaptivePanelDecision decision;
    decision.placement = preferred;
    decision.presentation = UMI_ADAPTIVE_PRESENTATION_INLINE;
    if (size_class == UMI_DESIGN_SIZE_COMPACT && preferred != UMI_UI_PLACEMENT_CENTRE) {
        decision.presentation = UMI_ADAPTIVE_PRESENTATION_OVERLAY;
        decision.placement = UMI_UI_PLACEMENT_FLOATING;
    }
    return decision;
}
