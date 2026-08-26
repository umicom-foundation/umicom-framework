/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_adaptive_property.c
 *
 * PURPOSE:
 *   Validate describe one responsive property path and its inherited base value.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_property.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveProperty p;
    CHECK(umi_designer_adaptive_property_init(&p,"layout.columns","2",1)==UMI_STATUS_OK);
    CHECK(p.inheritable);
    return 0;
}
