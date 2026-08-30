/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/navigation_adaptation.c
 *
 * PURPOSE:
 *   Resolve navigation capacity, overflow and pattern for a concrete viewport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_adaptation.h"

/* Budget navigation items by size class while keeping the underlying model unchanged. */
UmiStatus umi_adaptive_navigation_adaptation_resolve(const UmiAdaptiveNavigationModel *model,
                                                     UmiDesignSizeClass size_class,
                                                     UmiAdaptiveInputMode input_mode,
                                                     UmiAdaptiveNavigationDecision *out_decision)
{
    size_t capacity;
    if (model == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    capacity = size_class == UMI_DESIGN_SIZE_COMPACT ? 4U :
               size_class == UMI_DESIGN_SIZE_MEDIUM ? 6U :
               size_class == UMI_DESIGN_SIZE_EXPANDED ? 10U : UMI_ADAPTIVE_MAX_ITEMS;
    out_decision->pattern = umi_adaptive_navigation_pattern_select(size_class, input_mode);
    out_decision->visible_items = model->count < capacity ? model->count : capacity;
    out_decision->overflow_items = model->count - out_decision->visible_items;
    return UMI_STATUS_OK;
}
