/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/grid_adaptation.c
 *
 * PURPOSE:
 *   Adapt grid presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/grid_adaptation.h"

/* Cap visible grid columns by size class and expose overflow through scrolling. */
UmiAdaptiveGridDecision umi_adaptive_grid_adaptation_resolve(size_t column_count, UmiDesignSizeClass size_class)
{
    UmiAdaptiveGridDecision decision;
    size_t budget = size_class == UMI_DESIGN_SIZE_COMPACT ? 3U :
                    size_class == UMI_DESIGN_SIZE_MEDIUM ? 6U :
                    size_class == UMI_DESIGN_SIZE_EXPANDED ? 10U : 16U;
    decision.visible_columns = column_count < budget ? column_count : budget;
    decision.horizontal_scroll = column_count > decision.visible_columns;
    decision.condensed_headers = size_class <= UMI_DESIGN_SIZE_MEDIUM;
    return decision;
}
