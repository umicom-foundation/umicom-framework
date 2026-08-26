/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_orientation_change.c
 *
 * PURPOSE:
 *   Validate represent and apply deterministic orientation changes to logical viewport and safe-area state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/orientation_change.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveOrientationChange change;
    UmiAdaptiveViewport v = {390,844}; UmiAdaptiveInsets s = {44,0,34,0};
    CHECK(umi_adaptive_orientation_change_rotate(v,s,1,&change)==UMI_STATUS_OK);
    CHECK(change.viewport.width==844 && change.safe_area.right==44);
    return 0;
}
