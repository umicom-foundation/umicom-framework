/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_navigation_adaptation.c
 *
 * PURPOSE:
 *   Validate resolve navigation capacity, overflow and pattern for a concrete viewport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveNavigationModel model;
    UmiAdaptiveNavigationDecision decision;
    size_t i;
    umi_adaptive_navigation_model_init(&model);
    for (i = 0U; i < 6U; ++i) { UmiAdaptiveNavigationItem item; char id[8]; id[0]=(char)('a'+(int)i); id[1]='\0'; CHECK(umi_adaptive_navigation_item_init(&item,id,id,id,1U,0)==UMI_STATUS_OK); CHECK(umi_adaptive_navigation_model_add(&model,&item)==UMI_STATUS_OK); }
    CHECK(umi_adaptive_navigation_adaptation_resolve(&model, UMI_DESIGN_SIZE_COMPACT, UMI_ADAPTIVE_INPUT_TOUCH, &decision) == UMI_STATUS_OK);
    CHECK(decision.visible_items == 4U && decision.overflow_items == 2U);
    return 0;
}
