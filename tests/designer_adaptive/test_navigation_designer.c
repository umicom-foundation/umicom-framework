/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_navigation_designer.c
 *
 * PURPOSE:
 *   Validate own adaptive navigation-model selection and reorder operations for visual authoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/navigation_designer.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveNavigationModel m; UmiAdaptiveNavigationItem a,b; UmiDesignerNavigationDesigner d; umi_adaptive_navigation_model_init(&m);
    CHECK(umi_adaptive_navigation_item_init(&a,"a","A","a",1U,0)==UMI_STATUS_OK); CHECK(umi_adaptive_navigation_item_init(&b,"b","B","b",1U,0)==UMI_STATUS_OK);
    CHECK(umi_adaptive_navigation_model_add(&m,&a)==UMI_STATUS_OK); CHECK(umi_adaptive_navigation_model_add(&m,&b)==UMI_STATUS_OK);
    CHECK(umi_designer_navigation_designer_init(&d,&m)==UMI_STATUS_OK); CHECK(umi_designer_navigation_designer_move(&d,1U,0U)==UMI_STATUS_OK); CHECK(d.model.items[0].item_id[0]=='b');
    return 0;
}
