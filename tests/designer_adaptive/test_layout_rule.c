/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_layout_rule.c
 *
 * PURPOSE:
 *   Validate describe one component layout rule scoped to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/layout_rule.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveLayoutRule r;
    CHECK(umi_designer_layout_rule_init(&r,"rule","chart",UMI_DESIGN_SIZE_MEDIUM,UMI_DESIGN_SIZE_WIDE,2U,UMI_ADAPTIVE_PRESENTATION_INLINE)==UMI_STATUS_OK);
    CHECK(umi_designer_layout_rule_matches(&r,UMI_DESIGN_SIZE_EXPANDED));
    return 0;
}
