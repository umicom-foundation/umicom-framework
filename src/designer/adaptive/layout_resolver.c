/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/layout_resolver.c
 *
 * PURPOSE:
 *   Resolve authored adaptive layout rules into a compact preview/runtime layout summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_resolver.h"
#include <string.h>

/* Aggregate active rules without mutating the authored adaptive document. */
UmiStatus umi_designer_layout_resolver_resolve(const UmiDesignerAdaptiveLayoutRuleSet *set,
                                               UmiDesignSizeClass size_class,
                                               UmiDesignerResolvedLayout *out_layout)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || out_layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_layout, 0, sizeof *out_layout);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        const UmiDesignerAdaptiveLayoutRule *rule = &set->rules[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_designer_layout_rule_matches(rule, size_class)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->presentation == UMI_ADAPTIVE_PRESENTATION_HIDDEN) continue;
        ++out_layout->visible_components;
        out_layout->total_column_span += rule->column_span;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->presentation == UMI_ADAPTIVE_PRESENTATION_OVERLAY) ++out_layout->overlay_components;
    }
    return UMI_STATUS_OK;
}
