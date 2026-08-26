/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/layout_resolver.c
 *
 * PURPOSE:
 *   Resolve authored adaptive layout rules into a compact preview/runtime layout summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_resolver.h"
#include <string.h>

/* Aggregate active rules without mutating the authored adaptive document. */
UmiStatus umi_designer_layout_resolver_resolve(const UmiDesignerAdaptiveLayoutRuleSet *set,
                                               UmiDesignSizeClass size_class,
                                               UmiDesignerResolvedLayout *out_layout)
{
    size_t index;
    if (set == NULL || out_layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_layout, 0, sizeof *out_layout);
    for (index = 0U; index < set->count; ++index) {
        const UmiDesignerAdaptiveLayoutRule *rule = &set->rules[index];
        if (!umi_designer_layout_rule_matches(rule, size_class)) continue;
        if (rule->presentation == UMI_ADAPTIVE_PRESENTATION_HIDDEN) continue;
        ++out_layout->visible_components;
        out_layout->total_column_span += rule->column_span;
        if (rule->presentation == UMI_ADAPTIVE_PRESENTATION_OVERLAY) ++out_layout->overlay_components;
    }
    return UMI_STATUS_OK;
}
