/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/layout_rule_set.c
 *
 * PURPOSE:
 *   Collect unique adaptive layout rules for a page, form or application shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_rule_set.h"
#include <string.h>

/* Reset the bounded layout-rule set. */
void umi_designer_layout_rule_set_init(UmiDesignerAdaptiveLayoutRuleSet *set)
{
    if (set != NULL) memset(set, 0, sizeof *set);
}

/* Add unique rules while preserving authored order. */
UmiStatus umi_designer_layout_rule_set_add(UmiDesignerAdaptiveLayoutRuleSet *set,
                                           const UmiDesignerAdaptiveLayoutRule *rule)
{
    size_t index;
    if (set == NULL || rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < set->count; ++index)
        if (strcmp(set->rules[index].rule_id, rule->rule_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (set->count >= UMI_DESIGNER_ADAPTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->rules[set->count++] = *rule;
    return UMI_STATUS_OK;
}

/* Count active rules for designer diagnostics and preview budgets. */
size_t umi_designer_layout_rule_set_match_count(const UmiDesignerAdaptiveLayoutRuleSet *set,
                                                UmiDesignSizeClass size_class)
{
    size_t index, count = 0U;
    if (set == NULL) return 0U;
    for (index = 0U; index < set->count; ++index)
        if (umi_designer_layout_rule_matches(&set->rules[index], size_class)) ++count;
    return count;
}
