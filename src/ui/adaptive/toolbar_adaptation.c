/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/toolbar_adaptation.c
 *
 * PURPOSE:
 *   Adapt toolbar presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/toolbar_adaptation.h"

/* Reduce toolbar density while preserving commands through overflow presentation. */
UmiAdaptiveToolbarDecision umi_adaptive_toolbar_adaptation_resolve(size_t command_count, UmiDesignSizeClass size_class)
{
    UmiAdaptiveToolbarDecision decision;
    size_t budget = size_class == UMI_DESIGN_SIZE_COMPACT ? 3U :
                    size_class == UMI_DESIGN_SIZE_MEDIUM ? 6U : 12U;
    decision.visible_commands = command_count < budget ? command_count : budget;
    decision.overflow = command_count > decision.visible_commands;
    decision.labels_visible = size_class >= UMI_DESIGN_SIZE_EXPANDED;
    return decision;
}
