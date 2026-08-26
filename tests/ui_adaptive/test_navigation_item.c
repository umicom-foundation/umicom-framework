/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_navigation_item.c
 *
 * PURPOSE:
 *   Validate define renderer-neutral navigation items with command routing and priority metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_item.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveNavigationItem item;
    CHECK(umi_adaptive_navigation_item_init(&item, "home", "Home", "app.home", 100U, 1) == UMI_STATUS_OK);
    CHECK(item.pinned && item.priority == 100U);
    return 0;
}
