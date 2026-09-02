/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/layout_rule.h
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

#ifndef UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RULE_H
#define UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RULE_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer adaptive layout rule data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAdaptiveLayoutRule {
    char rule_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    char component_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignSizeClass minimum_class;
    UmiDesignSizeClass maximum_class;
    uint16_t column_span;
    UmiAdaptivePresentation presentation;
} UmiDesignerAdaptiveLayoutRule;
/* Initialise one adaptive layout rule. */
UmiStatus umi_designer_layout_rule_init(UmiDesignerAdaptiveLayoutRule *rule,
                                        const char *rule_id,
                                        const char *component_id,
                                        UmiDesignSizeClass minimum_class,
                                        UmiDesignSizeClass maximum_class,
                                        uint16_t column_span,
                                        UmiAdaptivePresentation presentation);
/* Return whether the rule applies to the active canonical size class. */
int umi_designer_layout_rule_matches(const UmiDesignerAdaptiveLayoutRule *rule,
                                     UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
