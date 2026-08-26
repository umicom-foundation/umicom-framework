/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_layout_rule_set.c
 *
 * PURPOSE:
 *   Validate collect unique adaptive layout rules for a page, form or application shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_rule_set.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveLayoutRuleSet set; UmiDesignerAdaptiveLayoutRule r;
    umi_designer_layout_rule_set_init(&set);
    CHECK(umi_designer_layout_rule_init(&r,"r","form",UMI_DESIGN_SIZE_COMPACT,UMI_DESIGN_SIZE_WIDE,1U,UMI_ADAPTIVE_PRESENTATION_INLINE)==UMI_STATUS_OK);
    CHECK(umi_designer_layout_rule_set_add(&set,&r)==UMI_STATUS_OK);
    CHECK(umi_designer_layout_rule_set_match_count(&set,UMI_DESIGN_SIZE_MEDIUM)==1U);
    return 0;
}
