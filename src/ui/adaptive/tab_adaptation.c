/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/tab_adaptation.c
 *
 * PURPOSE:
 *   Adapt tab presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/tab_adaptation.h"

/* Preserve all tabs logically while limiting simultaneously visible tab chrome. */
UmiAdaptiveTabDecision umi_adaptive_tab_adaptation_resolve(size_t tab_count, UmiDesignSizeClass size_class)
{
    UmiAdaptiveTabDecision decision;
    size_t budget = size_class == UMI_DESIGN_SIZE_COMPACT ? 2U :
                    size_class == UMI_DESIGN_SIZE_MEDIUM ? 4U : 8U;
    decision.visible_tabs = tab_count < budget ? tab_count : budget;
    decision.scrollable = tab_count > decision.visible_tabs;
    decision.compact_titles = size_class <= UMI_DESIGN_SIZE_MEDIUM;
    return decision;
}
