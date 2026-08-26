/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/form_adaptation.c
 *
 * PURPOSE:
 *   Adapt form presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/form_adaptation.h"

/* Reduce forms to one touch-friendly column on compact views and scale upward on workstations. */
UmiAdaptiveFormDecision umi_adaptive_form_adaptation_resolve(UmiDesignSizeClass size_class, UmiAdaptiveInputMode input_mode)
{
    UmiAdaptiveFormDecision decision;
    decision.columns = size_class == UMI_DESIGN_SIZE_COMPACT ? 1U :
                       size_class == UMI_DESIGN_SIZE_MEDIUM ? 2U :
                       size_class == UMI_DESIGN_SIZE_EXPANDED ? 3U : 4U;
    decision.labels_above = size_class <= UMI_DESIGN_SIZE_MEDIUM || input_mode == UMI_ADAPTIVE_INPUT_TOUCH;
    decision.full_width_actions = size_class == UMI_DESIGN_SIZE_COMPACT;
    return decision;
}
