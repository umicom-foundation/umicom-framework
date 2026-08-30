/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/layout_rule_set.h
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

#ifndef UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RULE_SET_H
#define UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RULE_SET_H
#include "umicom/designer/adaptive/layout_rule.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveLayoutRuleSet {
    UmiDesignerAdaptiveLayoutRule rules[UMI_DESIGNER_ADAPTIVE_MAX_ITEMS];
    size_t count;
} UmiDesignerAdaptiveLayoutRuleSet;
/* Initialise an empty layout-rule set. */
void umi_designer_layout_rule_set_init(UmiDesignerAdaptiveLayoutRuleSet *set);
/* Add one unique adaptive layout rule. */
UmiStatus umi_designer_layout_rule_set_add(UmiDesignerAdaptiveLayoutRuleSet *set,
                                           const UmiDesignerAdaptiveLayoutRule *rule);
/* Count rules active for one canonical size class. */
size_t umi_designer_layout_rule_set_match_count(const UmiDesignerAdaptiveLayoutRuleSet *set,
                                                UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
