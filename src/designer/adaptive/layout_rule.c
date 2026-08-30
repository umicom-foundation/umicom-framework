/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/layout_rule.c
 *
 * PURPOSE:
 *   Describe one component layout rule scoped to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_rule.h"
#include <string.h>

/* Store one scoped layout rule after validating its class interval and span. */
UmiStatus umi_designer_layout_rule_init(UmiDesignerAdaptiveLayoutRule *rule,
                                        const char *rule_id,
                                        const char *component_id,
                                        UmiDesignSizeClass minimum_class,
                                        UmiDesignSizeClass maximum_class,
                                        uint16_t column_span,
                                        UmiAdaptivePresentation presentation)
{
    UmiStatus status;
    if (rule == NULL || rule_id == NULL || component_id == NULL || minimum_class > maximum_class || column_span == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(rule, 0, sizeof *rule);
    status = umi_designer_adaptive_copy_text(rule->rule_id, sizeof rule->rule_id, rule_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(rule->component_id, sizeof rule->component_id, component_id);
    if (status != UMI_STATUS_OK) return status;
    rule->minimum_class = minimum_class;
    rule->maximum_class = maximum_class;
    rule->column_span = column_span;
    rule->presentation = presentation;
    return UMI_STATUS_OK;
}

/* Match the current class using the shared adaptive size-range helper. */
int umi_designer_layout_rule_matches(const UmiDesignerAdaptiveLayoutRule *rule,
                                     UmiDesignSizeClass size_class)
{
    return rule != NULL && umi_designer_adaptive_size_in_range(size_class, rule->minimum_class, rule->maximum_class);
}
