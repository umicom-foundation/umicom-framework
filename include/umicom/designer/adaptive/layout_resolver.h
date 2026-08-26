/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/layout_resolver.h
 *
 * PURPOSE:
 *   Resolve authored adaptive layout rules into a compact preview/runtime layout summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RESOLVER_H
#define UMICOM_DESIGNER_ADAPTIVE_LAYOUT_RESOLVER_H
#include "umicom/designer/adaptive/layout_rule_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerResolvedLayout {
    size_t visible_components;
    size_t overlay_components;
    uint32_t total_column_span;
} UmiDesignerResolvedLayout;
/* Resolve a rule set into active component and column-span totals. */
UmiStatus umi_designer_layout_resolver_resolve(const UmiDesignerAdaptiveLayoutRuleSet *set,
                                               UmiDesignSizeClass size_class,
                                               UmiDesignerResolvedLayout *out_layout);

#ifdef __cplusplus
}
#endif
#endif
