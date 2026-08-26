/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_navigation_model.c
 *
 * PURPOSE:
 *   Validate maintain a bounded canonical navigation model independent of renderer presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_model.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveNavigationModel model;
    UmiAdaptiveNavigationItem item;
    umi_adaptive_navigation_model_init(&model);
    CHECK(umi_adaptive_navigation_item_init(&item, "home", "Home", "app.home", 100U, 1) == UMI_STATUS_OK);
    CHECK(umi_adaptive_navigation_model_add(&model, &item) == UMI_STATUS_OK);
    CHECK(umi_adaptive_navigation_model_find(&model, "home") != NULL);
    return 0;
}
