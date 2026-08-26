/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/chart_adaptation.c
 *
 * PURPOSE:
 *   Adapt chart presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/chart_adaptation.h"

/* Preserve chart content while progressively reducing secondary chrome. */
UmiAdaptiveChartDecision umi_adaptive_chart_adaptation_resolve(UmiDesignSizeClass size_class)
{
    UmiAdaptiveChartDecision decision;
    decision.legend_visible = size_class >= UMI_DESIGN_SIZE_EXPANDED;
    decision.axis_labels_compact = size_class <= UMI_DESIGN_SIZE_MEDIUM;
    decision.minimum_height = size_class == UMI_DESIGN_SIZE_COMPACT ? 220 : 320;
    return decision;
}
