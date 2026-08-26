/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_adaptive_variant.c
 *
 * PURPOSE:
 *   Validate define one responsive design variant scoped by breakpoint and orientation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_variant.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveVariant v;
    CHECK(umi_designer_adaptive_variant_init(&v,"tablet","desktop","medium",UMI_DESIGN_SIZE_MEDIUM,UMI_ADAPTIVE_ORIENTATION_LANDSCAPE,50U)==UMI_STATUS_OK);
    CHECK(v.priority==50U);
    return 0;
}
