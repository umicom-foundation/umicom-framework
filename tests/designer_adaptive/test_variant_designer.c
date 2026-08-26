/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_variant_designer.c
 *
 * PURPOSE:
 *   Validate own adaptive-variant selection and insertion for the visual application designer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_designer.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerVariantDesigner d; UmiDesignerAdaptiveVariant v; umi_designer_variant_designer_init(&d);
    CHECK(umi_designer_adaptive_variant_init(&v,"phone","","compact",UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_ORIENTATION_PORTRAIT,1U)==UMI_STATUS_OK);
    CHECK(umi_designer_variant_designer_add(&d,&v)==UMI_STATUS_OK); CHECK(d.variants.count==1U);
    return 0;
}
