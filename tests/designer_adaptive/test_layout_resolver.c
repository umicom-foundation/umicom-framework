/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_layout_resolver.c
 *
 * PURPOSE:
 *   Validate resolve authored adaptive layout rules into a compact preview/runtime layout summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_resolver.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveLayoutRuleSet set; UmiDesignerAdaptiveLayoutRule r; UmiDesignerResolvedLayout out;
    umi_designer_layout_rule_set_init(&set);
    CHECK(umi_designer_layout_rule_init(&r,"r","panel",UMI_DESIGN_SIZE_COMPACT,UMI_DESIGN_SIZE_WIDE,2U,UMI_ADAPTIVE_PRESENTATION_OVERLAY)==UMI_STATUS_OK);
    CHECK(umi_designer_layout_rule_set_add(&set,&r)==UMI_STATUS_OK);
    CHECK(umi_designer_layout_resolver_resolve(&set,UMI_DESIGN_SIZE_COMPACT,&out)==UMI_STATUS_OK);
    CHECK(out.visible_components==1U && out.overlay_components==1U);
    return 0;
}
